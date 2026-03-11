"""
在 Jupyter Notebook 中直接运行的可视化代码
直接复制以下代码到 Jupyter Notebook 单元格中运行
"""

import os
import sys
import glob

try:
    import matplotlib.pyplot as plt
    import matplotlib.image as mpimg
    # 在 Jupyter Notebook 中使用时，取消下面这行的注释
    # %matplotlib inline
except ImportError:
    print("警告: 需要安装 matplotlib 库")
    print("请运行: !pip install matplotlib")
    sys.exit(1)


def find_visualization_files(vis_dir):
    """查找所有可视化文件"""
    if not os.path.exists(vis_dir):
        print(f"错误: 可视化目录不存在: {vis_dir}")
        return []
    
    image_extensions = ['*.png', '*.jpg', '*.jpeg']
    image_files = []
    
    for ext in image_extensions:
        image_files.extend(glob.glob(os.path.join(vis_dir, ext)))
        image_files.extend(glob.glob(os.path.join(vis_dir, '**', ext), recursive=True))
    
    image_files.sort()
    return image_files


def display_images(image_files, max_images_per_row=2):
    """显示图片"""
    if not image_files:
        print("未找到任何图片文件")
        return
    
    num_images = len(image_files)
    num_rows = (num_images + max_images_per_row - 1) // max_images_per_row
    
    fig, axes = plt.subplots(num_rows, max_images_per_row, 
                            figsize=(16, 8 * num_rows))
    
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
    
    for idx in range(num_images, num_rows * max_images_per_row):
        row = idx // max_images_per_row
        col = idx % max_images_per_row
        ax = axes[row][col] if num_rows > 1 else axes[col]
        ax.axis('off')
    
    plt.tight_layout()
    plt.show()


def view_training_results(vis_dir='/cache/output/visualizations', max_per_row=2):
    """
    查看训练结果可视化图表
    
    参数:
        vis_dir: 可视化文件目录路径
        max_per_row: 每行显示的最大图片数
    """
    print("=" * 80)
    print("训练结果可视化图表查看器")
    print("=" * 80)
    print(f"可视化目录: {vis_dir}")
    print("=" * 80)
    
    image_files = find_visualization_files(vis_dir)
    
    if image_files:
        print(f"\n找到 {len(image_files)} 个可视化文件，正在显示...\n")
        for img in image_files:
            print(f"  - {os.path.basename(img)}")
        print()
        display_images(image_files, max_per_row)
    else:
        print("\n未找到任何可视化文件")
        print(f"请检查目录: {vis_dir}")
        print("\n提示: 如果训练已完成，可视化文件通常在以下位置:")
        print("  - /cache/output/visualizations/ (ModelArts环境)")
        print("  - ./output/visualizations/ (本地环境)")


# ========== 在 Jupyter Notebook 中运行以下代码 ==========

# 方法1: 查看 ModelArts 训练结果（默认）
view_training_results(vis_dir='/cache/output/visualizations')

# 方法2: 查看本地训练结果（取消注释下面这行）
# view_training_results(vis_dir='./output/visualizations')

# 方法3: 自定义路径
# view_training_results(vis_dir='你的路径', max_per_row=3)

