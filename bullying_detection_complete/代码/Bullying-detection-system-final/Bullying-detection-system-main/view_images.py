#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
可视化训练结果图表
View Training Result Visualizations

用于查看训练过程中生成的各种可视化图表

作者: 三英战ICT团队
"""

import os
import sys
import argparse
import glob
from pathlib import Path

try:
    from PIL import Image
    import matplotlib.pyplot as plt
    import matplotlib.image as mpimg
except ImportError:
    print("警告: 需要安装 matplotlib 和 Pillow 库")
    print("请运行: pip install matplotlib pillow")
    sys.exit(1)


def find_visualization_files(vis_dir):
    """查找所有可视化文件"""
    if not os.path.exists(vis_dir):
        print(f"错误: 可视化目录不存在: {vis_dir}")
        return []
    
    # 查找所有图片文件
    image_extensions = ['*.png', '*.jpg', '*.jpeg']
    image_files = []
    
    for ext in image_extensions:
        image_files.extend(glob.glob(os.path.join(vis_dir, ext)))
        image_files.extend(glob.glob(os.path.join(vis_dir, '**', ext), recursive=True))
    
    # 按文件名排序
    image_files.sort()
    
    return image_files


def display_images(image_files, max_images_per_row=2, separate=False):
    """显示图片
    
    Args:
        image_files: 图片文件路径列表
        max_images_per_row: 每行显示的最大图片数（separate=False时使用）
        separate: 是否每张图片单独显示在一个画布上
    """
    if not image_files:
        print("未找到任何图片文件")
        return
    
    if separate:
        # 每张图片单独显示
        for idx, img_path in enumerate(image_files, 1):
            try:
                img = mpimg.imread(img_path)
                # 根据图片尺寸自动调整画布大小
                height, width = img.shape[:2]
                # 保持宽高比，但限制最大尺寸
                max_width = 20
                max_height = 12
                scale = min(max_width / (width/100), max_height / (height/100), 1.0)
                fig_width = max(12, width/100 * scale)
                fig_height = max(8, height/100 * scale)
                
                plt.figure(figsize=(fig_width, fig_height))
                plt.imshow(img)
                plt.axis('off')
                plt.title(os.path.basename(img_path), fontsize=14, fontweight='bold', pad=10)
                plt.tight_layout()
                plt.show()
                print(f"\n[{idx}/{len(image_files)}] {os.path.basename(img_path)}")
                print("=" * 80)
            except Exception as e:
                print(f"无法加载图片 {img_path}: {e}")
    else:
        # 原来的方式：多张图片在一个画布上
        num_images = len(image_files)
        num_rows = (num_images + max_images_per_row - 1) // max_images_per_row
        
        fig, axes = plt.subplots(num_rows, max_images_per_row, 
                                figsize=(16, 8 * num_rows))
        
        # 如果只有一行，确保 axes 是二维数组
        if num_rows == 1:
            axes = axes.reshape(1, -1)
        elif num_images == 1:
            axes = [[axes]]
        
        for idx, img_path in enumerate(image_files):
            row = idx // max_images_per_row
            col = idx % max_images_per_row
            
            try:
                img = mpimg.imread(img_path)
                ax = axes[row][col] if num_rows > 1 else axes[col]
                ax.imshow(img)
                ax.axis('off')
                ax.set_title(os.path.basename(img_path), fontsize=10, fontweight='bold')
            except Exception as e:
                print(f"无法加载图片 {img_path}: {e}")
        
        # 隐藏多余的子图
        for idx in range(num_images, num_rows * max_images_per_row):
            row = idx // max_images_per_row
            col = idx % max_images_per_row
            ax = axes[row][col] if num_rows > 1 else axes[col]
            ax.axis('off')
        
        plt.tight_layout()
        plt.show()


def list_images(image_files):
    """列出所有图片文件"""
    if not image_files:
        print("未找到任何图片文件")
        return
    
    print(f"\n找到 {len(image_files)} 个可视化文件:\n")
    print("=" * 80)
    
    for idx, img_path in enumerate(image_files, 1):
        file_size = os.path.getsize(img_path) / 1024  # KB
        print(f"{idx:3d}. {os.path.basename(img_path)}")
        print(f"     路径: {img_path}")
        print(f"     大小: {file_size:.2f} KB")
        print()


def parse_args():
    """解析命令行参数"""
    parser = argparse.ArgumentParser(description='查看训练结果可视化图表')
    
    parser.add_argument('--vis_dir', type=str, 
                       default='./output/visualizations',
                       help='可视化文件目录路径')
    parser.add_argument('--mode', type=str, default='display',
                       choices=['display', 'list'],
                       help='显示模式: display(显示图片) 或 list(仅列出文件)')
    parser.add_argument('--max_per_row', type=int, default=2,
                       help='每行显示的最大图片数')
    parser.add_argument('--separate', action='store_true',
                       help='每张图片单独显示在一个画布上')
    
    args = parser.parse_args()
    return args


def main():
    """主函数"""
    args = parse_args()
    
    print("=" * 80)
    print("训练结果可视化图表查看器")
    print("=" * 80)
    print(f"可视化目录: {args.vis_dir}")
    print(f"模式: {args.mode}")
    print("=" * 80)
    
    # 查找图片文件
    image_files = find_visualization_files(args.vis_dir)
    
    if args.mode == 'list':
        list_images(image_files)
    else:
        if image_files:
            print(f"\n找到 {len(image_files)} 个可视化文件，正在显示...")
            display_images(image_files, args.max_per_row, separate=args.separate)
        else:
            print("\n未找到任何可视化文件")
            print(f"请检查目录: {args.vis_dir}")
            print("\n提示: 如果训练已完成，可视化文件通常在以下位置:")
            print("  - ./output/visualizations/")
            print("  - /cache/output/visualizations/ (ModelArts环境)")


if __name__ == "__main__":
    main()

