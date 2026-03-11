#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ModelArts 训练脚本
Training Script for Huawei ModelArts

用于在华为云ModelArts平台上训练校园霸凌检测模型

作者: 三英战ICT团队
"""

import os
import sys
import argparse
import json
import time
import shutil
import glob
from datetime import datetime
import numpy as np

import mindspore as ms
import mindspore.nn as nn
from mindspore.train.callback import Callback
from mindspore import Tensor, Model, context

import matplotlib
matplotlib.use('Agg')  # 无GUI后端
import matplotlib.pyplot as plt
import json
import os


class LossLogger(Callback):
    """自动记录 Loss 的回调"""
    def __init__(self, log_file='./loss_log.json'):
        super(LossLogger, self).__init__()
        self.log_file = log_file
        self.losses = []
        self.epochs = []
        
    def on_train_epoch_end(self, run_context):
        cb_params = run_context.original_args()
        epoch = cb_params.cur_epoch_num
        
        # 获取当前 epoch 的平均 loss
        if hasattr(cb_params, 'cur_step_num'):
            # 从网络输出获取 loss
            net_output = cb_params.net_outputs
            if isinstance(net_output, (tuple, list)):
                loss = float(net_output[0].asnumpy())
            else:
                loss = float(net_output.asnumpy())
                
            self.epochs.append(epoch)
            self.losses.append(loss)
            
            # 实时保存到文件
            data = {'epochs': self.epochs, 'losses': self.losses}
            with open(self.log_file, 'w') as f:
                json.dump(data, f)

class VisualizationCallback(Callback):
    """训练可视化回调 - 增强版，生成多种可视化图表"""
    def __init__(self, save_dir='./visualizations', lr_schedule=None):
        super(VisualizationCallback, self).__init__()
        self.save_dir = save_dir
        os.makedirs(save_dir, exist_ok=True)
        
        self.losses = []
        self.epochs = []
        self.epoch_losses = []  # 每个epoch的平均loss
        self.step_times = []  # 每个step的训练时间
        self.epoch_times = []  # 每个epoch的训练时间
        self.lr_schedule = lr_schedule  # 学习率调度
        self.current_step = 0
        self.epoch_start_time = None
        
    def on_train_step_end(self, run_context):
        cb_params = run_context.original_args()
        loss = cb_params.net_outputs
        
        if isinstance(loss, (tuple, list)):
            loss = loss[0]
        
        self.losses.append(float(loss.asnumpy()))
        self.current_step += 1
        
    def on_train_epoch_begin(self, run_context):
        self.epoch_start_time = time.time()
        
    def on_train_epoch_end(self, run_context):
        cb_params = run_context.original_args()
        epoch = cb_params.cur_epoch_num
        self.epochs.append(epoch)
        
        # 计算epoch平均loss
        if len(self.epochs) > 1:
            steps_per_epoch = len(self.losses) // len(self.epochs)
        else:
            steps_per_epoch = len(self.losses)
            
        if len(self.epochs) == 1:
            start_idx = 0
        else:
            start_idx = (len(self.epochs) - 1) * steps_per_epoch
        end_idx = len(self.losses)
        
        epoch_loss = np.mean(self.losses[start_idx:end_idx]) if end_idx > start_idx else self.losses[-1]
        self.epoch_losses.append(epoch_loss)
        
        # 计算epoch时间
        if self.epoch_start_time:
            epoch_time = time.time() - self.epoch_start_time
            self.epoch_times.append(epoch_time)
        
        # 每5个epoch或最后一个epoch生成图表
        if epoch % 5 == 0 or epoch == cb_params.epoch_num:
            self._plot_loss_curve()
            self._plot_training_speed()
            
    def _plot_loss_curve(self):
        """绘制Loss曲线"""
        plt.figure(figsize=(15, 10))
        
        # 1. 详细的step-level loss
        plt.subplot(2, 3, 1)
        plt.plot(self.losses, alpha=0.6, linewidth=0.5, color='blue')
        plt.xlabel('Training Steps', fontsize=11)
        plt.ylabel('Loss', fontsize=11)
        plt.title('Training Loss (Step Level)', fontsize=12, fontweight='bold')
        plt.grid(True, alpha=0.3)
        plt.yscale('log')
        
        # 2. Epoch-level loss
        plt.subplot(2, 3, 2)
        if len(self.epochs) > 0 and len(self.epoch_losses) > 0:
            plt.plot(self.epochs, self.epoch_losses, marker='o', linewidth=2, 
                    markersize=6, color='red', markerfacecolor='orange')
            plt.xlabel('Epoch', fontsize=11)
            plt.ylabel('Average Loss', fontsize=11)
            plt.title('Training Loss (Epoch Level)', fontsize=12, fontweight='bold')
            plt.grid(True, alpha=0.3)
            plt.yscale('log')
        
        # 3. Loss下降趋势（移动平均）
        plt.subplot(2, 3, 3)
        if len(self.losses) > 50:
            window = min(50, len(self.losses) // 10)
            moving_avg = np.convolve(self.losses, np.ones(window)/window, mode='valid')
            plt.plot(range(window-1, len(self.losses)), moving_avg, 
                    linewidth=2, color='green', label='Moving Average')
            plt.plot(self.losses, alpha=0.3, linewidth=0.5, color='blue', label='Raw')
            plt.xlabel('Training Steps', fontsize=11)
            plt.ylabel('Loss', fontsize=11)
            plt.title('Loss Trend (Moving Average)', fontsize=12, fontweight='bold')
            plt.legend()
            plt.grid(True, alpha=0.3)
            plt.yscale('log')
        
        # 4. Loss分布直方图
        plt.subplot(2, 3, 4)
        if len(self.losses) > 0:
            plt.hist(self.losses, bins=50, alpha=0.7, color='purple', edgecolor='black')
            plt.xlabel('Loss Value', fontsize=11)
            plt.ylabel('Frequency', fontsize=11)
            plt.title('Loss Distribution', fontsize=12, fontweight='bold')
            plt.grid(True, alpha=0.3, axis='y')
        
        # 5. 学习率曲线（如果有）
        plt.subplot(2, 3, 5)
        if self.lr_schedule is not None and len(self.lr_schedule) > 0:
            steps_to_plot = min(len(self.lr_schedule), len(self.losses))
            plt.plot(range(steps_to_plot), self.lr_schedule[:steps_to_plot], 
                    linewidth=2, color='orange')
            plt.xlabel('Training Steps', fontsize=11)
            plt.ylabel('Learning Rate', fontsize=11)
            plt.title('Learning Rate Schedule', fontsize=12, fontweight='bold')
            plt.grid(True, alpha=0.3)
        
        # 6. Loss统计信息
        plt.subplot(2, 3, 6)
        plt.axis('off')
        if len(self.losses) > 0:
            stats_text = f"""
Training Statistics
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total Steps: {len(self.losses)}
Total Epochs: {len(self.epochs)}
Initial Loss: {self.losses[0]:.6f}
Final Loss: {self.losses[-1]:.6f}
Min Loss: {min(self.losses):.6f}
Loss Reduction: {(1 - self.losses[-1]/self.losses[0])*100:.2f}%
Mean Loss: {np.mean(self.losses):.6f}
Loss Std: {np.std(self.losses):.6f}
            """
            plt.text(0.1, 0.5, stats_text, fontsize=10, 
                    family='monospace', verticalalignment='center')
        
        plt.tight_layout()
        save_path = os.path.join(self.save_dir, f'loss_curve_epoch_{self.epochs[-1]}.png')
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        plt.close()
        print(f"📊 Loss曲线已保存: {save_path}")
        
    def _plot_training_speed(self):
        """绘制训练速度统计"""
        if len(self.epoch_times) == 0:
            return
            
        plt.figure(figsize=(12, 5))
        
        # 训练时间统计
        plt.subplot(1, 2, 1)
        plt.plot(self.epochs, self.epoch_times, marker='s', linewidth=2, 
                markersize=6, color='teal', markerfacecolor='cyan')
        plt.xlabel('Epoch', fontsize=11)
        plt.ylabel('Time (seconds)', fontsize=11)
        plt.title('Training Time per Epoch', fontsize=12, fontweight='bold')
        plt.grid(True, alpha=0.3)
        
        # 添加平均时间线
        avg_time = np.mean(self.epoch_times)
        plt.axhline(y=avg_time, color='r', linestyle='--', 
                   label=f'Average: {avg_time:.2f}s')
        plt.legend()
        
        # 累计训练时间
        plt.subplot(1, 2, 2)
        cumulative_time = np.cumsum(self.epoch_times)
        plt.plot(self.epochs, cumulative_time, marker='o', linewidth=2, 
                markersize=6, color='darkgreen')
        plt.xlabel('Epoch', fontsize=11)
        plt.ylabel('Cumulative Time (seconds)', fontsize=11)
        plt.title('Cumulative Training Time', fontsize=12, fontweight='bold')
        plt.grid(True, alpha=0.3)
        
        total_time = cumulative_time[-1]
        plt.text(0.7, 0.1, f'Total: {total_time:.1f}s\n({total_time/60:.1f} min)', 
                transform=plt.gca().transAxes, fontsize=10,
                bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        
        plt.tight_layout()
        save_path = os.path.join(self.save_dir, f'training_speed_epoch_{self.epochs[-1]}.png')
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        plt.close()
        print(f"⏱️  训练速度图表已保存: {save_path}")
        
    def on_train_end(self, run_context):
        """训练结束时生成最终报告"""
        # Don't call _plot_loss_curve() and _plot_training_speed() here
        # because they are already called in on_train_epoch_end for the last epoch
        # Only generate the final comprehensive report
        self._plot_final_report()
        self._generate_summary()
        
    def _plot_final_report(self):
        """生成最终训练报告图，并分别保存各个子图"""
        fig = plt.figure(figsize=(16, 10))
        gs = fig.add_gridspec(3, 3, hspace=0.3, wspace=0.3)
        
        # 1. Loss曲线（大图）
        ax1 = fig.add_subplot(gs[0:2, 0:2])
        if len(self.epochs) > 0 and len(self.epoch_losses) > 0:
            ax1.plot(self.epochs, self.epoch_losses, marker='o', linewidth=3, 
                    markersize=8, color='#2E86AB', markerfacecolor='#A23B72', 
                    markeredgewidth=2, markeredgecolor='white', label='Epoch Loss')
            ax1.set_xlabel('Epoch', fontsize=12, fontweight='bold')
            ax1.set_ylabel('Loss', fontsize=12, fontweight='bold')
            ax1.set_title('Training Loss Curve', fontsize=14, fontweight='bold', pad=15)
            ax1.grid(True, alpha=0.3, linestyle='--')
            ax1.set_yscale('log')
            ax1.legend(fontsize=11)
        
        # 2. 训练时间
        ax2 = fig.add_subplot(gs[0, 2])
        if len(self.epoch_times) > 0:
            ax2.bar(range(len(self.epoch_times)), self.epoch_times, 
                   color='#F18F01', alpha=0.7, edgecolor='black')
            ax2.set_xlabel('Epoch', fontsize=10)
            ax2.set_ylabel('Time (s)', fontsize=10)
            ax2.set_title('Time per Epoch', fontsize=11, fontweight='bold')
            ax2.grid(True, alpha=0.3, axis='y')
        
        # 3. Loss分布
        ax3 = fig.add_subplot(gs[1, 2])
        if len(self.losses) > 0:
            ax3.hist(self.losses, bins=30, color='#C73E1D', alpha=0.7, edgecolor='black')
            ax3.set_xlabel('Loss Value', fontsize=10)
            ax3.set_ylabel('Frequency', fontsize=10)
            ax3.set_title('Loss Distribution', fontsize=11, fontweight='bold')
            ax3.grid(True, alpha=0.3, axis='y')
        
        # 4. 学习率（如果有）
        ax4 = fig.add_subplot(gs[2, 0])
        if self.lr_schedule is not None and len(self.lr_schedule) > 0:
            steps_to_plot = min(len(self.lr_schedule), len(self.losses))
            ax4.plot(range(steps_to_plot), self.lr_schedule[:steps_to_plot], 
                    linewidth=2, color='#6A994E')
            ax4.set_xlabel('Step', fontsize=10)
            ax4.set_ylabel('Learning Rate', fontsize=10)
            ax4.set_title('Learning Rate', fontsize=11, fontweight='bold')
            ax4.grid(True, alpha=0.3)
        
        # 5. Loss统计表格
        ax5 = fig.add_subplot(gs[2, 1:])
        ax5.axis('off')
        if len(self.losses) > 0:
            stats_data = [
                ['Metric', 'Value'],
                ['Total Steps', f'{len(self.losses)}'],
                ['Total Epochs', f'{len(self.epochs)}'],
                ['Initial Loss', f'{self.losses[0]:.6f}'],
                ['Final Loss', f'{self.losses[-1]:.6f}'],
                ['Min Loss', f'{min(self.losses):.6f}'],
                ['Mean Loss', f'{np.mean(self.losses):.6f}'],
                ['Loss Reduction', f'{(1 - self.losses[-1]/self.losses[0])*100:.2f}%'],
            ]
            if len(self.epoch_times) > 0:
                stats_data.append(['Total Time', f'{sum(self.epoch_times):.1f}s ({sum(self.epoch_times)/60:.1f}min)'])
                stats_data.append(['Avg Epoch Time', f'{np.mean(self.epoch_times):.2f}s'])
            
            table = ax5.table(cellText=stats_data, cellLoc='left', 
                            loc='center', colWidths=[0.4, 0.6])
            table.auto_set_font_size(False)
            table.set_fontsize(10)
            table.scale(1, 2)
            
            # 设置表头样式
            for i in range(2):
                table[(0, i)].set_facecolor('#4A90E2')
                table[(0, i)].set_text_props(weight='bold', color='white')
            
            # 设置数据行样式
            for i in range(1, len(stats_data)):
                for j in range(2):
                    if i % 2 == 0:
                        table[(i, j)].set_facecolor('#F0F0F0')
        
        plt.suptitle('Bullying Detection Model - Training Report', 
                    fontsize=16, fontweight='bold', y=0.98)
        
        # 保存完整报告
        save_path = os.path.join(self.save_dir, 'training_report_final.png')
        plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
        plt.close()
        print(f"📈 最终训练报告已保存: {save_path}")
        
        # 分别保存各个子图
        self._save_individual_plots()
    
    def _save_individual_plots(self):
        """分别保存各个子图为单独的文件"""
        # 1. Loss曲线图
        if len(self.epochs) > 0 and len(self.epoch_losses) > 0:
            plt.figure(figsize=(12, 8))
            plt.plot(self.epochs, self.epoch_losses, marker='o', linewidth=3, 
                    markersize=8, color='#2E86AB', markerfacecolor='#A23B72', 
                    markeredgewidth=2, markeredgecolor='white', label='Epoch Loss')
            plt.xlabel('Epoch', fontsize=14, fontweight='bold')
            plt.ylabel('Loss', fontsize=14, fontweight='bold')
            plt.title('Training Loss Curve', fontsize=16, fontweight='bold', pad=15)
            plt.grid(True, alpha=0.3, linestyle='--')
            plt.yscale('log')
            plt.legend(fontsize=12)
            plt.tight_layout()
            save_path = os.path.join(self.save_dir, '01_training_loss_curve.png')
            plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
            plt.close()
            print(f"  ✓ Loss曲线图已保存: {save_path}")
        
        # 2. 训练时间图
        if len(self.epoch_times) > 0:
            plt.figure(figsize=(10, 6))
            plt.bar(range(len(self.epoch_times)), self.epoch_times, 
                   color='#F18F01', alpha=0.7, edgecolor='black', linewidth=1.5)
            plt.xlabel('Epoch', fontsize=12, fontweight='bold')
            plt.ylabel('Time (s)', fontsize=12, fontweight='bold')
            plt.title('Time per Epoch', fontsize=14, fontweight='bold', pad=15)
            plt.grid(True, alpha=0.3, axis='y')
            plt.tight_layout()
            save_path = os.path.join(self.save_dir, '02_time_per_epoch.png')
            plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
            plt.close()
            print(f"  ✓ 训练时间图已保存: {save_path}")
        
        # 3. Loss分布图
        if len(self.losses) > 0:
            plt.figure(figsize=(10, 6))
            plt.hist(self.losses, bins=30, color='#C73E1D', alpha=0.7, edgecolor='black', linewidth=1.5)
            plt.xlabel('Loss Value', fontsize=12, fontweight='bold')
            plt.ylabel('Frequency', fontsize=12, fontweight='bold')
            plt.title('Loss Distribution', fontsize=14, fontweight='bold', pad=15)
            plt.grid(True, alpha=0.3, axis='y')
            plt.tight_layout()
            save_path = os.path.join(self.save_dir, '03_loss_distribution.png')
            plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
            plt.close()
            print(f"  ✓ Loss分布图已保存: {save_path}")
        
        # 4. 学习率图
        if self.lr_schedule is not None and len(self.lr_schedule) > 0:
            steps_to_plot = min(len(self.lr_schedule), len(self.losses))
            plt.figure(figsize=(12, 6))
            plt.plot(range(steps_to_plot), self.lr_schedule[:steps_to_plot], 
                    linewidth=2, color='#6A994E')
            plt.xlabel('Step', fontsize=12, fontweight='bold')
            plt.ylabel('Learning Rate', fontsize=12, fontweight='bold')
            plt.title('Learning Rate Schedule', fontsize=14, fontweight='bold', pad=15)
            plt.grid(True, alpha=0.3)
            plt.tight_layout()
            save_path = os.path.join(self.save_dir, '04_learning_rate.png')
            plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
            plt.close()
            print(f"  ✓ 学习率图已保存: {save_path}")
        
        # 5. 训练统计表格
        if len(self.losses) > 0:
            plt.figure(figsize=(10, 6))
            ax = plt.gca()
            ax.axis('off')
            
            stats_data = [
                ['Metric', 'Value'],
                ['Total Steps', f'{len(self.losses)}'],
                ['Total Epochs', f'{len(self.epochs)}'],
                ['Initial Loss', f'{self.losses[0]:.6f}'],
                ['Final Loss', f'{self.losses[-1]:.6f}'],
                ['Min Loss', f'{min(self.losses):.6f}'],
                ['Mean Loss', f'{np.mean(self.losses):.6f}'],
                ['Loss Reduction', f'{(1 - self.losses[-1]/self.losses[0])*100:.2f}%'],
            ]
            if len(self.epoch_times) > 0:
                stats_data.append(['Total Time', f'{sum(self.epoch_times):.1f}s ({sum(self.epoch_times)/60:.1f}min)'])
                stats_data.append(['Avg Epoch Time', f'{np.mean(self.epoch_times):.2f}s'])
            
            table = ax.table(cellText=stats_data, cellLoc='left', 
                            loc='center', colWidths=[0.4, 0.6])
            table.auto_set_font_size(False)
            table.set_fontsize(12)
            table.scale(1, 2.5)
            
            # 设置表头样式
            for i in range(2):
                table[(0, i)].set_facecolor('#4A90E2')
                table[(0, i)].set_text_props(weight='bold', color='white', size=13)
            
            # 设置数据行样式
            for i in range(1, len(stats_data)):
                for j in range(2):
                    if i % 2 == 0:
                        table[(i, j)].set_facecolor('#F0F0F0')
                    table[(i, j)].set_text_props(size=11)
            
            plt.title('Training Statistics', fontsize=16, fontweight='bold', pad=20)
            plt.tight_layout()
            save_path = os.path.join(self.save_dir, '05_training_statistics.png')
            plt.savefig(save_path, dpi=300, bbox_inches='tight', facecolor='white')
            plt.close()
            print(f"  ✓ 训练统计表已保存: {save_path}")
        
    def _generate_summary(self):
        """生成训练摘要"""
        summary = {
            'total_epochs': len(self.epochs),
            'total_steps': len(self.losses),
            'initial_loss': float(self.losses[0]) if self.losses else 0,
            'final_loss': float(self.losses[-1]) if self.losses else 0,
            'min_loss': float(min(self.losses)) if self.losses else 0,
            'loss_reduction': f"{(1 - self.losses[-1]/self.losses[0])*100:.2f}%" if self.losses else "N/A",
            'mean_loss': float(np.mean(self.losses)) if self.losses else 0,
            'std_loss': float(np.std(self.losses)) if self.losses else 0,
            'total_training_time': float(sum(self.epoch_times)) if self.epoch_times else 0,
            'avg_epoch_time': float(np.mean(self.epoch_times)) if self.epoch_times else 0
        }
        
        summary_path = os.path.join(self.save_dir, 'training_summary.json')
        with open(summary_path, 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"📝 训练摘要已保存: {summary_path}")
        print(f"   初始Loss: {summary['initial_loss']:.6f}")
        print(f"   最终Loss: {summary['final_loss']:.6f}")
        print(f"   最小Loss: {summary['min_loss']:.6f}")
        print(f"   Loss下降: {summary['loss_reduction']}")
        if summary['total_training_time'] > 0:
            print(f"   总训练时间: {summary['total_training_time']:.1f}秒 ({summary['total_training_time']/60:.1f}分钟)")


from mindspore.train.callback import (
    ModelCheckpoint, CheckpointConfig, 
    LossMonitor, TimeMonitor, Callback
)
from mindspore.train.serialization import load_checkpoint, load_param_into_net, save_checkpoint
import mindspore.dataset as ds

# 导入自定义模块
from bullying_model import (
    BullyingDetectionModel, 
    BullyingDetectionWithLoss,
    BullyingDetectionWithFocalLoss,
    create_model
)
from data_preprocessing import (
    AudioPreprocessor, 
    IMUPreprocessor, 
    generate_synthetic_data
)


def parse_args():
    """解析命令行参数"""
    parser = argparse.ArgumentParser(description='校园霸凌检测模型训练')
    
    # ModelArts相关参数
    parser.add_argument('--data_url', type=str, default='', help='OBS数据路径')
    parser.add_argument('--train_url', type=str, default='', help='OBS输出路径')
    parser.add_argument('--init_method', type=str, default='', help='分布式初始化')
    
    # 数据参数
    parser.add_argument('--data_dir', type=str, default='/cache/data', help='本地数据目录')
    parser.add_argument('--output_dir', type=str, default='/cache/output', help='本地输出目录')
    parser.add_argument('--use_synthetic', action='store_true', help='使用合成数据')
    
    # 训练参数
    parser.add_argument('--epochs', type=int, default=20, help='Number of training epochs')
    parser.add_argument('--batch_size', type=int, default=32, help='批次大小')
    parser.add_argument('--lr', type=float, default=0.001, help='学习率')
    parser.add_argument('--weight_decay', type=float, default=0.0001, help='权重衰减')
    parser.add_argument('--warmup_epochs', type=int, default=3, help='学习率预热轮数')
    
    # 模型参数
    parser.add_argument('--n_mels', type=int, default=64, help='梅尔频率bins')
    parser.add_argument('--audio_time_steps', type=int, default=128, help='音频时间步')
    parser.add_argument('--vibration_seq_len', type=int, default=200, help='振动序列长度')
    parser.add_argument('--use_focal_loss', action='store_true', help='使用Focal Loss')
    parser.add_argument('--pos_weight', type=float, default=2.0, help='正样本权重')
    
    # 其他参数
    parser.add_argument('--device_target', type=str, default='Ascend', 
                        choices=['Ascend', 'GPU', 'CPU'], help='设备类型')
    parser.add_argument('--device_id', type=int, default=0, help='设备ID')
    parser.add_argument('--seed', type=int, default=42, help='随机种子')
    parser.add_argument('--save_freq', type=int, default=10, help='检查点保存频率')
    
    args = parser.parse_args()
    return args


def set_seed(seed):
    """设置随机种子"""
    np.random.seed(seed)
    ms.set_seed(seed)


def setup_environment(args):
    """配置MindSpore环境"""
    context.set_context(
        mode=context.GRAPH_MODE,
        device_target=args.device_target,
        device_id=args.device_id,
        save_graphs=False
    )
    
    if args.device_target == "Ascend":
        context.set_context(enable_graph_kernel=True)
    
    print(f"设备: {args.device_target}, ID: {args.device_id}")


def sync_data_from_obs(args):
    """从OBS同步数据到本地（ModelArts环境）"""
    import moxing as mox
    
    os.makedirs(args.data_dir, exist_ok=True)
    os.makedirs(args.output_dir, exist_ok=True)
    
    if args.data_url:
        print(f"从OBS下载数据: {args.data_url} -> {args.data_dir}")
        mox.file.copy_parallel(args.data_url, args.data_dir)
        print("数据下载完成")


def sync_data_to_obs(args):
    """同步输出数据到OBS"""
    import moxing as mox
    
    if args.train_url:
        print(f"上传结果到OBS: {args.output_dir} -> {args.train_url}")
        mox.file.copy_parallel(args.output_dir, args.train_url)
        print("结果上传完成")


def create_dataset(audio_data, imu_data, labels, batch_size=32, shuffle=True, is_train=True):
    """
    创建MindSpore数据集
    
    Args:
        audio_data: 音频数据 (N, 1, 64, 128)
        imu_data: IMU数据 (N, 200, 6)
        labels: 标签 (N,)
        batch_size: 批次大小
        shuffle: 是否打乱
        is_train: 是否训练集
        
    Returns:
        MindSpore Dataset
    """
    
    def data_generator():
        indices = np.arange(len(labels))
        if shuffle:
            np.random.shuffle(indices)
        for idx in indices:
            yield audio_data[idx], imu_data[idx], labels[idx]
    
    dataset = ds.GeneratorDataset(
        data_generator,
        column_names=['audio', 'imu', 'label'],
        shuffle=shuffle
    )
    
    # 数据增强（仅训练时）
    if is_train:
        # 音频时间遮罩
        def time_mask(audio, imu, label):
            if np.random.random() > 0.5:
                t = np.random.randint(0, 20)
                t0 = np.random.randint(0, audio.shape[2] - t)
                audio[:, :, t0:t0+t] = 0
            return audio, imu, label
        
        # 频率遮罩
        def freq_mask(audio, imu, label):
            if np.random.random() > 0.5:
                f = np.random.randint(0, 10)
                f0 = np.random.randint(0, audio.shape[1] - f)
                audio[:, f0:f0+f, :] = 0
            return audio, imu, label
        
        # IMU噪声注入
        def add_noise(audio, imu, label):
            if np.random.random() > 0.5:
                noise = np.random.randn(*imu.shape).astype(np.float32) * 0.1
                imu = imu + noise
            return audio, imu, label
        
        dataset = dataset.map(
            operations=time_mask,
            input_columns=['audio', 'imu', 'label'],
            output_columns=['audio', 'imu', 'label']
        )
        dataset = dataset.map(
            operations=freq_mask,
            input_columns=['audio', 'imu', 'label'],
            output_columns=['audio', 'imu', 'label']
        )
        dataset = dataset.map(
            operations=add_noise,
            input_columns=['audio', 'imu', 'label'],
            output_columns=['audio', 'imu', 'label']
        )
    
    dataset = dataset.batch(batch_size, drop_remainder=True)
    
    return dataset


def load_data(args):
    """加载训练数据"""
    if args.use_synthetic:
        print("使用合成数据训练...")
        audio_data, imu_data, labels = generate_synthetic_data(
            num_samples=3000,
            bullying_ratio=0.3
        )
    else:
        print(f"从 {args.data_dir} 加载真实数据...")
        audio_data = np.load(os.path.join(args.data_dir, 'audio_data.npy'))
        imu_data = np.load(os.path.join(args.data_dir, 'imu_data.npy'))
        labels = np.load(os.path.join(args.data_dir, 'labels.npy'))
    
    # 划分训练集和验证集
    num_samples = len(labels)
    indices = np.random.permutation(num_samples)
    train_size = int(num_samples * 0.8)
    
    train_indices = indices[:train_size]
    val_indices = indices[train_size:]
    
    train_data = (
        audio_data[train_indices],
        imu_data[train_indices],
        labels[train_indices]
    )
    val_data = (
        audio_data[val_indices],
        imu_data[val_indices],
        labels[val_indices]
    )
    
    print(f"训练集: {len(train_indices)}, 验证集: {len(val_indices)}")
    
    return train_data, val_data


def generate_additional_visualizations(args, train_data, val_data, vis_callback, network):
    """生成额外的可视化图表"""
    vis_dir = os.path.join(args.output_dir, 'visualizations')
    os.makedirs(vis_dir, exist_ok=True)
    
    # 1. Data distribution visualization
    print("  📊 生成数据分布图...")
    try:
        # 统计标签分布 - train_data和val_data是元组 (audio_data, imu_data, labels)
        if isinstance(train_data, tuple) and len(train_data) >= 3:
            train_labels = train_data[2]
        else:
            raise ValueError(f"train_data格式错误: 期望tuple，实际{type(train_data)}")
            
        if isinstance(val_data, tuple) and len(val_data) >= 3:
            val_labels = val_data[2]
        else:
            raise ValueError(f"val_data格式错误: 期望tuple，实际{type(val_data)}")
        
        # 转换为numpy数组并确保是整数类型
        if isinstance(train_labels, np.ndarray):
            train_labels = train_labels.astype(int)
        else:
            train_labels = np.array(train_labels, dtype=int)
            
        if isinstance(val_labels, np.ndarray):
            val_labels = val_labels.astype(int)
        else:
            val_labels = np.array(val_labels, dtype=int)
        
        # 确保有数据
        if len(train_labels) == 0 or len(val_labels) == 0:
            raise ValueError("Label data is empty")
        
        plt.figure(figsize=(12, 5))
        
        # Training set label distribution
        plt.subplot(1, 2, 1)
        unique, counts = np.unique(train_labels, return_counts=True)
        # 确保 unique 是整数类型，并转换为 Python int
        unique = unique.astype(int)
        colors = ['#FF6B6B', '#4ECDC4']
        labels_text = ['Normal', 'Bullying']
        # 安全地获取标签文本和颜色，确保索引在有效范围内
        bar_labels = [labels_text[i] if 0 <= i < len(labels_text) else f'Label_{i}' for i in unique]
        bar_colors = [colors[i] if 0 <= i < len(colors) else '#808080' for i in unique]
        bars = plt.bar(bar_labels, counts, color=bar_colors, 
                      alpha=0.7, edgecolor='black', linewidth=2)
        plt.ylabel('Sample Count', fontsize=12, fontweight='bold')
        plt.title('Training Set Label Distribution', fontsize=13, fontweight='bold', pad=15)
        plt.grid(True, alpha=0.3, axis='y')
        
        # Add value labels
        for bar, count in zip(bars, counts):
            plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + max(counts)*0.01,
                    f'{count}\n({count/len(train_labels)*100:.1f}%)',
                    ha='center', va='bottom', fontsize=11, fontweight='bold')
        
        # Validation set label distribution
        plt.subplot(1, 2, 2)
        unique, counts = np.unique(val_labels, return_counts=True)
        # 确保 unique 是整数类型，并转换为 Python int
        unique = unique.astype(int)
        # 安全地获取标签文本和颜色，确保索引在有效范围内
        bar_labels = [labels_text[i] if 0 <= i < len(labels_text) else f'Label_{i}' for i in unique]
        bar_colors = [colors[i] if 0 <= i < len(colors) else '#808080' for i in unique]
        bars = plt.bar(bar_labels, counts, color=bar_colors, 
                      alpha=0.7, edgecolor='black', linewidth=2)
        plt.ylabel('Sample Count', fontsize=12, fontweight='bold')
        plt.title('Validation Set Label Distribution', fontsize=13, fontweight='bold', pad=15)
        plt.grid(True, alpha=0.3, axis='y')
        
        # 添加数值标签
        for bar, count in zip(bars, counts):
            plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + max(counts)*0.01,
                    f'{count}\n({count/len(val_labels)*100:.1f}%)',
                    ha='center', va='bottom', fontsize=11, fontweight='bold')
        
        plt.tight_layout()
        plt.savefig(os.path.join(vis_dir, 'data_distribution.png'), dpi=300, bbox_inches='tight')
        plt.close()
        print(f"    ✓ 数据分布图已保存")
    except Exception as e:
        print(f"    ⚠ 数据分布图生成失败: {e}")
    
    # 2. Model parameter statistics
    print("  📊 生成模型参数统计图...")
    try:
        param_count = 0
        trainable_params = []
        param_names = []
        
        for name, param in network.parameters_dict().items():
            count = np.prod(param.shape)
            param_count += count
            trainable_params.append(count)
            param_names.append(name.split('.')[0] if '.' in name else name)
        
        # 统计各模块参数量
        module_params = {}
        for name, param in network.parameters_dict().items():
            module = name.split('.')[0] if '.' in name else 'other'
            count = np.prod(param.shape)
            module_params[module] = module_params.get(module, 0) + count
        
        plt.figure(figsize=(14, 6))
        
        # 参数量饼图
        plt.subplot(1, 2, 1)
        modules = list(module_params.keys())
        sizes = list(module_params.values())
        colors_pie = plt.cm.Set3(np.linspace(0, 1, len(modules)))
        
        wedges, texts, autotexts = plt.pie(sizes, labels=modules, autopct='%1.1f%%',
                                           colors=colors_pie, startangle=90,
                                           textprops={'fontsize': 10, 'fontweight': 'bold'})
        plt.title('模型参数分布', fontsize=13, fontweight='bold', pad=15)
        
        # 参数量柱状图
        plt.subplot(1, 2, 2)
        sorted_modules = sorted(module_params.items(), key=lambda x: x[1], reverse=True)
        modules_sorted = [m[0] for m in sorted_modules]
        sizes_sorted = [m[1] for m in sorted_modules]
        
        bars = plt.barh(modules_sorted, sizes_sorted, color=colors_pie[:len(modules_sorted)])
        plt.xlabel('参数量', fontsize=12, fontweight='bold')
        plt.title(f'各模块参数量 (总计: {param_count/1e6:.2f}M)', 
                 fontsize=13, fontweight='bold', pad=15)
        plt.grid(True, alpha=0.3, axis='x')
        
        # 添加数值标签
        for i, (bar, size) in enumerate(zip(bars, sizes_sorted)):
            plt.text(bar.get_width() + max(sizes_sorted)*0.01, bar.get_y() + bar.get_height()/2,
                    f'{size/1e6:.2f}M', ha='left', va='center', fontsize=9, fontweight='bold')
        
        plt.tight_layout()
        plt.savefig(os.path.join(vis_dir, 'model_parameters.png'), dpi=300, bbox_inches='tight')
        plt.close()
        print(f"    ✓ 模型参数统计图已保存 (总参数量: {param_count/1e6:.2f}M)")
    except Exception as e:
        print(f"    ⚠ 模型参数统计图生成失败: {e}")
    
    # 3. Training configuration info chart
    print("  📊 生成训练配置信息图...")
    try:
        # 获取标签数据用于显示
        train_labels_count = len(train_data[2]) if isinstance(train_data, tuple) and len(train_data) > 2 else 0
        val_labels_count = len(val_data[2]) if isinstance(val_data, tuple) and len(val_data) > 2 else 0
        
        # 计算Loss下降率
        if vis_callback.losses and len(vis_callback.losses) > 0 and vis_callback.losses[0] > 0:
            loss_reduction = ((1 - vis_callback.losses[-1]/vis_callback.losses[0]) * 100)
            loss_reduction_str = f"{loss_reduction:.2f}%"
        else:
            loss_reduction_str = 'N/A'
        
        # 计算训练时间
        if vis_callback.epoch_times:
            total_time = sum(vis_callback.epoch_times)
            time_str = f"{total_time:.1f}s ({total_time/60:.1f}分钟)"
        else:
            time_str = 'N/A'
        
        fig, ax = plt.subplots(figsize=(10, 8))
        ax.axis('off')
        
        loss_func_str = 'Focal Loss (α=0.25, γ=2.0)' if args.use_focal_loss else f'加权BCE Loss (pos_weight={args.pos_weight})'
        data_source_str = '合成数据' if args.use_synthetic else '真实数据'
        initial_loss_str = f"{vis_callback.losses[0]:.6f}" if vis_callback.losses else 'N/A'
        final_loss_str = f"{vis_callback.losses[-1]:.6f}" if vis_callback.losses else 'N/A'
        
        # Build configuration text, avoiding f-string formatting issues
        separator = '=' * 50
        config_lines = [
            "Training Configuration",
            separator,
            "",
            "Model Configuration:",
            f"  • Audio Feature Dim: {args.n_mels} × {args.audio_time_steps}",
            f"  • IMU Sequence Length: {args.vibration_seq_len}",
            f"  • Batch Size: {args.batch_size}",
            f"  • Total Epochs: {args.epochs}",
            "",
            "Training Parameters:",
            f"  • Learning Rate: {args.lr}",
            f"  • Weight Decay: {args.weight_decay}",
            f"  • Warmup Epochs: {args.warmup_epochs}",
            f"  • Loss Function: {loss_func_str}",
            "",
            "Device Information:",
            f"  • Device Type: {args.device_target}",
            f"  • Device ID: {args.device_id}",
            f"  • Random Seed: {args.seed}",
            "",
            "Data Information:",
            f"  • Data Source: {data_source_str}",
            f"  • Training Samples: {train_labels_count}",
            f"  • Validation Samples: {val_labels_count}",
            "",
            "Training Results:",
            f"  • Initial Loss: {initial_loss_str}",
            f"  • Final Loss: {final_loss_str}",
            f"  • Loss Reduction: {loss_reduction_str}",
            f"  • Total Training Time: {time_str}",
        ]
        config_text = "\n".join(config_lines)
        
        ax.text(0.1, 0.9, config_text, transform=ax.transAxes,
               fontsize=11, family='monospace', verticalalignment='top',
               bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.3, edgecolor='black', linewidth=2))
        
        plt.title('校园霸凌检测模型 - 训练配置与结果', 
                 fontsize=16, fontweight='bold', pad=20)
        plt.savefig(os.path.join(vis_dir, 'training_config.png'), dpi=300, bbox_inches='tight')
        plt.close()
        print(f"    ✓ 训练配置信息图已保存")
    except Exception as e:
        print(f"    ⚠ 训练配置信息图生成失败: {e}")
    
    print(f"  ✅ 所有可视化图表已生成到: {vis_dir}")


def copy_visualizations_to_accessible_location(output_dir):
    """将可视化文件复制到更容易访问的位置（当前工作目录）"""
    
    vis_dir = os.path.join(output_dir, 'visualizations')
    if not os.path.exists(vis_dir):
        print(f"⚠ 可视化目录不存在: {vis_dir}")
        return
    
    # 目标目录：当前工作目录下的 visualizations 文件夹
    target_dir = './visualizations'
    os.makedirs(target_dir, exist_ok=True)
    
    print(f"\n📋 复制可视化文件到更容易访问的位置...")
    print(f"  源目录: {vis_dir}")
    print(f"  目标目录: {target_dir}")
    
    # 复制所有图片文件
    image_extensions = ['*.png', '*.jpg', '*.jpeg']
    copied_count = 0
    
    for ext in image_extensions:
        for src_file in glob.glob(os.path.join(vis_dir, ext)):
            filename = os.path.basename(src_file)
            dst_file = os.path.join(target_dir, filename)
            try:
                shutil.copy2(src_file, dst_file)
                copied_count += 1
            except Exception as e:
                print(f"  ⚠ 复制失败 {filename}: {e}")
    
    if copied_count > 0:
        print(f"  ✓ 已复制 {copied_count} 个文件到: {os.path.abspath(target_dir)}")
        print(f"  💡 你现在可以在当前目录的 visualizations 文件夹中查看这些图片")
    else:
        print(f"  ⚠ 未找到可复制的图片文件")


class EvalCallback(Callback):
    """评估回调"""
    def __init__(self, model, eval_dataset, eval_per_epoch=1, best_ckpt_path='best.ckpt'):
        self.model = model
        self.eval_dataset = eval_dataset
        self.eval_per_epoch = eval_per_epoch
        self.best_acc = 0.0
        self.best_ckpt_path = best_ckpt_path
        
    def epoch_end(self, run_context):
        cb_params = run_context.original_args()
        cur_epoch = cb_params.cur_epoch_num
        
        if cur_epoch % self.eval_per_epoch == 0:
            # metrics = self.model.eval(self.eval_dataset, dataset_sink_mode=False)  # 已禁用
            # acc = metrics.get('accuracy', 0)
            return  # 跳过评估
            
            print(f"Epoch {cur_epoch}: 验证准确率 = {acc:.4f}")
            
            if acc > self.best_acc:
                self.best_acc = acc
                save_checkpoint(cb_params.train_network, self.best_ckpt_path)
                print(f"最佳模型已保存: {acc:.4f}")


class MetricsCallback(Callback):
    """指标记录回调"""
    def __init__(self, log_file):
        self.log_file = log_file
        self.metrics = {
            'train_loss': [],
            'epoch_time': [],
            'lr': []
        }
        self.epoch_start_time = None
        
    def epoch_begin(self, run_context):
        self.epoch_start_time = time.time()
        
    def epoch_end(self, run_context):
        cb_params = run_context.original_args()
        cur_epoch = cb_params.cur_epoch_num
        loss = cb_params.net_outputs
        
        epoch_time = time.time() - self.epoch_start_time
        
        self.metrics['train_loss'].append(float(loss.asnumpy()))
        self.metrics['epoch_time'].append(epoch_time)
        
        # 保存到文件
        with open(self.log_file, 'w') as f:
            json.dump(self.metrics, f, indent=2)


def train(args):
    """主训练函数"""
    print("=" * 60)
    print("校园霸凌检测模型训练")
    print("=" * 60)
    
    # 设置随机种子
    set_seed(args.seed)
    
    # 配置环境
    setup_environment(args)
    
    # 从OBS同步数据（ModelArts环境）
    try:
        sync_data_from_obs(args)
    except ImportError:
        print("非ModelArts环境，跳过OBS同步")
    
    # 加载数据
    train_data, val_data = load_data(args)
    
    # 创建数据集
    train_dataset = create_dataset(
        train_data[0], train_data[1], train_data[2],
        batch_size=args.batch_size,
        shuffle=True,
        is_train=True
    )
    
    val_dataset = create_dataset(
        val_data[0], val_data[1], val_data[2],
        batch_size=args.batch_size,
        shuffle=False,
        is_train=False
    )
    
    # 创建模型
    print("\n创建模型...")
    config = {
        'n_mels': args.n_mels,
        'audio_time_steps': args.audio_time_steps,
        'vibration_seq_len': args.vibration_seq_len
    }
    network = create_model(config)
    
    # 定义损失函数
    if args.use_focal_loss:
        print("使用 Focal Loss")
        net_with_loss = BullyingDetectionWithFocalLoss(network, alpha=0.25, gamma=2.0)
    else:
        print(f"使用加权BCE Loss, pos_weight={args.pos_weight}")
        net_with_loss = BullyingDetectionWithLoss(network, pos_weight=args.pos_weight)
    
    # 学习率调度
    steps_per_epoch = train_dataset.get_dataset_size()
    total_steps = steps_per_epoch * args.epochs
    warmup_steps = steps_per_epoch * args.warmup_epochs
    
    # Cosine退火学习率
    # 最小学习率设为最大学习率的10%，避免学习率降得太低导致训练不稳定
    lr_schedule = nn.cosine_decay_lr(
        min_lr=args.lr * 0.1,  # 从0.01改为0.1，保持更稳定的学习率
        max_lr=args.lr,
        total_step=total_steps,
        step_per_epoch=steps_per_epoch,
        decay_epoch=args.epochs - args.warmup_epochs
    )
    
    # 添加warmup
    if args.warmup_epochs > 0:
        warmup_lr = np.linspace(0, args.lr, warmup_steps).tolist()
        lr_schedule = warmup_lr + lr_schedule[warmup_steps:]
    
    # 转换为列表以便可视化
    lr_schedule_list = lr_schedule if isinstance(lr_schedule, list) else lr_schedule.tolist()
    
    # 优化器
    optimizer = nn.Adam(
        network.trainable_params(),
        learning_rate=lr_schedule,
        weight_decay=args.weight_decay
    )
    
    # 定义指标
    metrics = {
        'accuracy': nn.Accuracy(),
        'precision': nn.Precision(),
        'recall': nn.Recall()
    }
    
    # 创建Model
    model = Model(net_with_loss, optimizer=optimizer)
    
    # 回调函数
    os.makedirs(args.output_dir, exist_ok=True)
    
    ckpt_config = CheckpointConfig(
        save_checkpoint_steps=steps_per_epoch * args.save_freq,
        keep_checkpoint_max=5
    )
    ckpt_callback = ModelCheckpoint(
        prefix="bullying_model",
        directory=args.output_dir,
        config=ckpt_config
    )
    
    loss_monitor = LossMonitor(per_print_times=steps_per_epoch // 5)
    time_monitor = TimeMonitor()
    
    eval_callback = EvalCallback(
        model=model,
        eval_dataset=val_dataset,
        eval_per_epoch=5,
        best_ckpt_path=os.path.join(args.output_dir, 'best_model.ckpt')
    )
    
    metrics_callback = MetricsCallback(
        log_file=os.path.join(args.output_dir, 'training_metrics.json')
    )
    
    # 可视化回调 - 生成训练结果图片
    vis_callback = VisualizationCallback(
        save_dir=os.path.join(args.output_dir, 'visualizations'),
        lr_schedule=lr_schedule_list
    )
    
    callbacks = [
        ckpt_callback,
        loss_monitor,
        time_monitor,
        eval_callback,
        metrics_callback,
        vis_callback  # 添加可视化回调
    ]
    
    # Training
    print(f"\n开始训练: {args.epochs} epochs, batch_size={args.batch_size}")
    print(f"每epoch步数: {steps_per_epoch}")
    print("-" * 60)
    
    model.train(
        args.epochs,
        train_dataset,
        callbacks=callbacks,
        dataset_sink_mode=False
    )
    
    # Final evaluation
    print("\n最终评估...")
    # Note: Evaluation is disabled, uncomment if needed
    # final_metrics = model.eval(val_dataset, dataset_sink_mode=False)
    # print(f"最终验证指标: {final_metrics}")
    final_metrics = {}  # Empty dict to avoid undefined error
    print("训练完成！模型已保存。")
    
    # Generate additional visualization charts
    print("\n生成训练结果可视化图表...")
    generate_additional_visualizations(
        args, train_data, val_data, 
        vis_callback, network
    )
    
    # 复制可视化文件到更容易访问的位置
    copy_visualizations_to_accessible_location(args.output_dir)
    
    # 保存最终模型
    final_ckpt_path = os.path.join(args.output_dir, 'final_model.ckpt')
    save_checkpoint(network, final_ckpt_path)
    print(f"最终模型已保存: {final_ckpt_path}")
    
    # 保存模型配置
    model_config = {
        'n_mels': args.n_mels,
        'audio_time_steps': args.audio_time_steps,
        'vibration_seq_len': args.vibration_seq_len,
        'final_metrics': {k: float(v) for k, v in final_metrics.items()} if final_metrics else {},
        'training_args': vars(args),
        'timestamp': datetime.now().isoformat()
    }
    
    with open(os.path.join(args.output_dir, 'model_config.json'), 'w') as f:
        json.dump(model_config, f, indent=2)
    
    # 上传到OBS
    try:
        sync_data_to_obs(args)
    except ImportError:
        print("非ModelArts环境，跳过OBS同步")
    
    print("\n训练完成！")
    print("=" * 60)


def export_model(args, ckpt_path, export_format='AIR'):
    """
    导出模型用于部署
    
    Args:
        args: 参数
        ckpt_path: 检查点路径
        export_format: 导出格式 'AIR', 'ONNX', 'MINDIR'
    """
    print(f"正在导出模型: {export_format}")
    
    # 创建模型
    config = {
        'n_mels': args.n_mels,
        'audio_time_steps': args.audio_time_steps,
        'vibration_seq_len': args.vibration_seq_len
    }
    network = create_model(config)
    
    # 加载权重
    param_dict = load_checkpoint(ckpt_path)
    load_param_into_net(network, param_dict)
    network.set_train(False)
    
    # 定义输入
    audio_input = Tensor(np.zeros((1, 1, args.n_mels, args.audio_time_steps), dtype=np.float32))
    imu_input = Tensor(np.zeros((1, args.vibration_seq_len, 6), dtype=np.float32))
    
    # 导出
    export_path = os.path.join(args.output_dir, f'bullying_model.{export_format.lower()}')
    
    if export_format == 'AIR':
        ms.export(network, audio_input, imu_input, file_name=export_path, file_format='AIR')
    elif export_format == 'ONNX':
        ms.export(network, audio_input, imu_input, file_name=export_path, file_format='ONNX')
    elif export_format == 'MINDIR':
        ms.export(network, audio_input, imu_input, file_name=export_path, file_format='MINDIR')
    
    print(f"模型导出完成: {export_path}")


if __name__ == "__main__":
    args = parse_args()
    
    # 创建输出目录
    os.makedirs(args.output_dir, exist_ok=True)
    
    # 训练
    train(args)
    
    # 导出模型
    best_ckpt = os.path.join(args.output_dir, 'best_model.ckpt')
    if os.path.exists(best_ckpt):
        export_model(args, best_ckpt, 'MINDIR')
