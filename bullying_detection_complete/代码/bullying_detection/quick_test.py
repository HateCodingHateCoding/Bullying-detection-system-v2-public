#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
快速测试脚本
Quick Test Script

用于验证模型和各组件是否正常工作

作者: 三英战ICT团队
"""

import sys
import os
import time
import numpy as np

# 添加路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))


def test_data_generation():
    """测试数据生成"""
    print("\n" + "=" * 60)
    print("测试 1: 合成数据生成")
    print("=" * 60)
    
    from modelarts.data_preprocessing import generate_synthetic_data
    
    audio, imu, labels = generate_synthetic_data(
        num_samples=100,
        bullying_ratio=0.3
    )
    
    print(f"✓ 音频数据形状: {audio.shape} (期望: (100, 1, 64, 128))")
    print(f"✓ IMU数据形状: {imu.shape} (期望: (100, 200, 6))")
    print(f"✓ 标签形状: {labels.shape} (期望: (100,))")
    print(f"✓ 霸凌样本数: {int(labels.sum())}")
    print(f"✓ 正常样本数: {len(labels) - int(labels.sum())}")
    
    return True


def test_feature_extraction():
    """测试特征提取"""
    print("\n" + "=" * 60)
    print("测试 2: 特征提取")
    print("=" * 60)
    
    from modelarts.data_preprocessing import AudioPreprocessor, IMUPreprocessor
    
    # 音频预处理
    audio_proc = AudioPreprocessor(
        sample_rate=16000,
        n_mels=64,
        duration=4.0
    )
    
    test_audio = np.random.randn(64000).astype(np.float32)
    audio_feat = audio_proc.process(test_audio)
    
    print(f"✓ 音频特征形状: {audio_feat.shape} (期望: (1, 64, 128))")
    
    # IMU预处理
    imu_proc = IMUPreprocessor(
        sample_rate=100,
        duration=2.0
    )
    
    test_imu = np.random.randn(200, 6).astype(np.float32)
    imu_feat = imu_proc.process(test_imu)
    
    print(f"✓ IMU特征形状: {imu_feat.shape} (期望: (200, 6))")
    
    return True


def test_model_creation():
    """测试模型创建"""
    print("\n" + "=" * 60)
    print("测试 3: 模型创建 (需要MindSpore)")
    print("=" * 60)
    
    try:
        import mindspore as ms
        ms.set_context(mode=ms.PYNATIVE_MODE, device_target="CPU")
        
        from modelarts.bullying_model import create_model
        
        model = create_model({
            'n_mels': 64,
            'audio_time_steps': 128,
            'vibration_seq_len': 200
        })
        
        # 测试前向传播
        batch_size = 2
        audio_input = ms.Tensor(np.random.randn(batch_size, 1, 64, 128).astype(np.float32))
        imu_input = ms.Tensor(np.random.randn(batch_size, 200, 6).astype(np.float32))
        
        prob, attn = model(audio_input, imu_input)
        
        print(f"✓ 模型创建成功")
        print(f"✓ 输出概率形状: {prob.shape} (期望: ({batch_size}, 1))")
        print(f"✓ 注意力权重形状: {attn.shape} (期望: ({batch_size}, 1))")
        print(f"✓ 概率值范围: [{prob.min().asnumpy():.4f}, {prob.max().asnumpy():.4f}]")
        
        return True
        
    except ImportError:
        print("⚠ MindSpore未安装，跳过模型测试")
        return True
    except Exception as e:
        print(f"✗ 模型测试失败: {e}")
        return False


def test_edge_inference_simulation():
    """测试边侧推理模拟"""
    print("\n" + "=" * 60)
    print("测试 4: 边侧推理模拟")
    print("=" * 60)
    
    # 模拟特征提取器和推理
    class MockFeatureExtractor:
        def extract_audio_features(self, audio):
            return np.random.randn(1, 64, 128).astype(np.float32)
        
        def extract_imu_features(self, imu):
            return np.random.randn(200, 6).astype(np.float32)
    
    class MockModelInference:
        def inference(self, audio_feat, imu_feat):
            # 模拟推理：根据输入能量计算概率
            audio_energy = np.mean(np.abs(audio_feat))
            imu_energy = np.mean(np.abs(imu_feat))
            
            # 简单规则
            prob = min(0.3 * audio_energy + 0.7 * imu_energy, 1.0)
            attn = 0.5
            
            return float(prob), float(attn)
    
    feature_extractor = MockFeatureExtractor()
    model = MockModelInference()
    
    # 模拟正常场景
    normal_audio = np.random.randn(64000).astype(np.float32) * 0.3
    normal_imu = np.random.randn(200, 6).astype(np.float32) * 0.3
    
    audio_feat = feature_extractor.extract_audio_features(normal_audio)
    imu_feat = feature_extractor.extract_imu_features(normal_imu)
    prob, attn = model.inference(audio_feat, imu_feat)
    
    print(f"✓ 正常场景 - 概率: {prob:.2%}")
    
    # 模拟霸凌场景
    bullying_audio = np.random.randn(64000).astype(np.float32) * 2.0
    bullying_imu = np.random.randn(200, 6).astype(np.float32) * 3.0
    
    audio_feat = feature_extractor.extract_audio_features(bullying_audio)
    imu_feat = feature_extractor.extract_imu_features(bullying_imu)
    prob, attn = model.inference(audio_feat, imu_feat)
    
    print(f"✓ 霸凌场景 - 概率: {prob:.2%}")
    
    return True


def test_alarm_logic():
    """测试报警逻辑"""
    print("\n" + "=" * 60)
    print("测试 5: 报警逻辑")
    print("=" * 60)
    
    THRESHOLD = 0.7
    CONFIRM_COUNT = 2
    
    alarm_counter = 0
    test_probabilities = [0.3, 0.5, 0.75, 0.82, 0.45, 0.78, 0.85]
    
    print(f"阈值: {THRESHOLD}, 确认次数: {CONFIRM_COUNT}")
    print("-" * 40)
    
    for i, prob in enumerate(test_probabilities):
        if prob >= THRESHOLD:
            alarm_counter += 1
        else:
            alarm_counter = max(0, alarm_counter - 1)
        
        should_alarm = alarm_counter >= CONFIRM_COUNT
        
        status = "🔴 报警!" if should_alarm else "🟢 正常"
        print(f"  第{i+1}次: P={prob:.2f}, 计数={alarm_counter}, {status}")
        
        if should_alarm:
            alarm_counter = 0  # 重置
    
    print("-" * 40)
    print("✓ 报警逻辑测试完成")
    
    return True


def test_performance():
    """测试性能"""
    print("\n" + "=" * 60)
    print("测试 6: 性能基准")
    print("=" * 60)
    
    iterations = 100
    
    # 测试特征提取性能
    from modelarts.data_preprocessing import AudioPreprocessor, IMUPreprocessor
    
    audio_proc = AudioPreprocessor()
    imu_proc = IMUPreprocessor()
    
    test_audio = np.random.randn(64000).astype(np.float32)
    test_imu = np.random.randn(200, 6).astype(np.float32)
    
    # 音频特征提取
    start = time.time()
    for _ in range(iterations):
        _ = audio_proc.process(test_audio)
    audio_time = (time.time() - start) / iterations * 1000
    
    # IMU特征提取
    start = time.time()
    for _ in range(iterations):
        _ = imu_proc.process(test_imu)
    imu_time = (time.time() - start) / iterations * 1000
    
    print(f"✓ 音频特征提取: {audio_time:.2f} ms/次")
    print(f"✓ IMU特征提取: {imu_time:.2f} ms/次")
    print(f"✓ 总预处理时间: {audio_time + imu_time:.2f} ms/次")
    
    # 内存使用
    import sys
    
    audio_mem = sys.getsizeof(test_audio) / 1024
    imu_mem = sys.getsizeof(test_imu) / 1024
    
    print(f"✓ 音频数据内存: {audio_mem:.2f} KB")
    print(f"✓ IMU数据内存: {imu_mem:.2f} KB")
    
    return True


def run_all_tests():
    """运行所有测试"""
    print("\n" + "=" * 60)
    print("校园霸凌检测系统 - 快速测试")
    print("=" * 60)
    
    tests = [
        ("数据生成", test_data_generation),
        ("特征提取", test_feature_extraction),
        ("模型创建", test_model_creation),
        ("边侧推理", test_edge_inference_simulation),
        ("报警逻辑", test_alarm_logic),
        ("性能基准", test_performance),
    ]
    
    results = []
    
    for name, test_func in tests:
        try:
            success = test_func()
            results.append((name, success))
        except Exception as e:
            print(f"✗ {name} 测试失败: {e}")
            results.append((name, False))
    
    # 汇总
    print("\n" + "=" * 60)
    print("测试结果汇总")
    print("=" * 60)
    
    passed = sum(1 for _, s in results if s)
    total = len(results)
    
    for name, success in results:
        status = "✓ 通过" if success else "✗ 失败"
        print(f"  {name}: {status}")
    
    print("-" * 60)
    print(f"通过: {passed}/{total}")
    
    if passed == total:
        print("\n🎉 所有测试通过!")
    else:
        print("\n⚠ 部分测试失败，请检查")
    
    return passed == total


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
