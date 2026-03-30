# 教学管理系统 (Education Management System)

<div align="center">

![Qt](https://img.shields.io/badge/Qt-6.8.3-41CD52?logo=qt&logoColor=white)
![C++](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus&logoColor=white)
![SQLite](https://img.shields.io/badge/SQLite-3-003B57?logo=sqlite&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)
![License](https://img.shields.io/badge/license-MIT-green)

</div>

---

## 📖 项目简介

教学管理系统是一款基于 **Qt6 + C++17** 开发的现代化桌面应用程序，专为教育机构设计。系统采用 **SQLite** 嵌入式数据库，实现了学生信息管理、课程安排、财务记录、荣誉墙展示等核心功能模块。

### ✨ 主要特性

- 🎨 **现代化 UI 设计**：采用 QSS 样式表，实现科技感十足的扁平化界面
- 📊 **数据可视化**：集成 Qt Charts，提供财务数据的饼图、折线图展示
- 🔐 **安全认证**：SHA-256 密码加密存储，支持记住密码和自动登录
- 💾 **本地数据库**：SQLite 嵌入式数据库，零配置部署
- 📷 **图片管理**：支持学生照片和荣誉墙图片的数据库存储与展示
- ⚙️ **可配置化**：支持数据库路径、主题、缓存等系统设置

---

## 🛠️ 技术栈

| 技术 | 版本 | 说明 |
|------|------|------|
| **Qt Framework** | 6.8.3 | 跨平台 C++ 图形用户界面应用程序开发框架 |
| **C++ Standard** | C++17 | 现代 C++ 标准，提供更简洁的语法和更强的功能 |
| **Database** | SQLite 3 | 轻量级嵌入式关系型数据库 |
| **Build System** | CMake | 跨平台的自动化构建工具 |
| **UI Components** | Qt Widgets | 传统但强大的桌面 UI 组件库 |
| **Charts** | Qt Charts | 2D 图表绘制库（饼图、折线图等） |
| **Stylesheets** | QSS | Qt Style Sheets，类似 CSS 的样式系统 |

### 核心 Qt 模块

- **Qt Widgets**：主窗口、对话框、表格、按钮等 UI 组件
- **Qt Sql**：数据库连接、SQL 查询、事务处理
- **Qt Charts**：数据可视化图表（`QChartView`、`QPieSeries`、`QLineSeries`）
- **Qt Gui**：图像处理、绘图功能

---

## 📦 功能模块

### 1. 学员信息管理
- ✅ 学生信息的增删改查
- ✅ 学生照片上传与显示（数据库存储）
- ✅ 表格展示与分页
- ✅ 数据验证与去重

### 2. 课程安排
- 📅 周视图课程表展示
- 📅 自定义周数和学年
- 📅 课程时间、地点、教师信息管理

### 3. 财务管理
- 💰 缴费记录管理（添加、删除、修改）
- 📊 数据可视化：
  - 饼图：按缴费类型统计占比
  - 折线图：缴费趋势分析
- 🔍 按学生、日期范围筛选

### 4. 荣誉墙
- 🏆 荣誉图片展示（网格布局）
- 🏆 图片添加、删除、修改
- 🏆 点击图片预览与交互

### 5. 系统设置
- ⚙️ 数据库路径配置
- ⚙️ 登录缓存与记住密码
- ⚙️ 主题切换（浅色/深色）
- ⚙️ 密码修改功能

### 6. 用户认证
- 🔐 登录对话框
- 🔐 密码 SHA-256 加密
- 🔐 自动登录（可选）
- 🔐 初始管理员账户创建

---

## 🚀 快速开始

### 环境要求

#### 必需软件

| 软件 | 版本 | 下载地址 |
|------|------|----------|
| **Qt Creator** | 6.0+ | [Qt 官网](https://www.qt.io/download) |
| **Qt Framework** | 6.8.3 | 通过 Qt Maintenance Tool 安装 |
| **CMake** | 3.16+ | [CMake 官网](https://cmake.org/download/) |
| **MinGW** | 13.1.0 (64-bit) | Qt 安装包自带 |

#### 推荐开发环境

- **Windows**: Qt Creator 12.0 + MinGW 13.1.0 64-bit
- **Linux**: Qt Creator 12.0 + GCC 11+
- **macOS**: Qt Creator 12.0 + Clang 14+

### 环境搭建

#### Windows (MinGW)

1. **下载并安装 Qt Online Installer**
   ```bash
   # 访问 https://www.qt.io/download-qt-installer
   # 下载 qt-online-installer-windows-x64-*.exe
   ```

2. **安装 Qt 6.8.3**
   - 运行安装程序
   - 选择自定义安装
   - 展开 `Qt 6.8.3` → 勾选 `MinGW 13.1.0 64-bit`
   - 展开 `Qt Creator` → 勾选最新版本
   - 完成安装

3. **验证安装**
   ```bash
   # 打开 Qt Creator
   # 工具 → 选项 → Kits → 检查编译器、Qt 版本、CMake 配置
   ```

#### Linux (Ubuntu/Debian)

```bash
# 安装 Qt 开发工具
sudo apt update
sudo apt install qt6-base-dev qt6-charts-dev cmake g++ -y

# 验证 Qt 版本
qmake6 --version
```

#### macOS

```bash
# 使用 Homebrew 安装
brew install qt@6 cmake

# 链接到系统路径
brew link --force qt@6
```

### 编译运行

#### 方法一：使用 Qt Creator（推荐）

1. **打开项目**
   - 启动 Qt Creator
   - 文件 → 打开文件或项目
   - 选择 `EdycationSystem/CMakeLists.txt`

2. **配置 Kit**
   - 工具 → 选项 → Kits
   - 确保选择了 Qt 6.8.3 和 MinGW 13.1.0

3. **构建项目**
   ```bash
   # 点击左下角 "构建" 按钮
   # 或使用快捷键 Ctrl+B
   ```

4. **运行程序**
   ```bash
   # 点击左下角 "运行" 按钮
   # 或使用快捷键 Ctrl+R
   ```

#### 方法二：命令行构建

```bash
# 进入项目目录
cd EdycationSystem

# 创建构建目录
mkdir build && cd build

# 配置 CMake（Windows）
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug

# 配置 CMake（Linux/macOS）
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 构建项目
cmake --build .

# 运行程序
./EdycationSystem  # Linux/macOS
EdycationSystem.exe  # Windows
```

### 数据库配置

#### 默认数据库路径

```
Windows: %APPDATA%/EdycationSystem/data/student_system.db
Linux: ~/.local/share/EdycationSystem/data/student_system.db
macOS: ~/Library/Application Support/EdycationSystem/data/student_system.db
```

#### 修改数据库路径

1. **通过系统设置界面**
   - 登录系统后，进入"系统设置"页面
   - 点击"浏览..."选择新的数据库文件路径
   - 点击"保存设置"

2. **手动修改配置文件**
   ```ini
   # config.ini 文件位置：应用程序同目录
   [Database]
   Path=C:/your/custom/path/student_system.db
   ```

#### 数据库表结构

系统包含以下数据表：

- **users**: 用户账户表（用户名、密码哈希）
- **studentInfo**: 学生信息表（学号、姓名、性别、生日等）
- **schedule**: 课程安排表（课程名、时间、地点等）
- **financialRecords**: 财务记录表（金额、缴费类型、日期等）
- **honorWall**: 荣誉墙表（图片数据、描述等）

### 默认登录凭据

```
用户名：admin
密码：admin123
```

⚠️ **安全提示**：首次登录后请立即修改默认密码！

---

## 📁 项目结构

```
EdycationSystem/
├── CMakeLists.txt              # CMake 构建配置
├── main.cpp                    # 程序入口
├── mainwindow.h/.cpp/.ui       # 主窗口（导航、页面切换）
├── logindialog.h/.cpp/.ui      # 登录对话框
├── databasemanager.h/.cpp      # 数据库单例管理
├── settings.h/.cpp             # 系统设置管理（单例）
├── settingswidget.h/.cpp/.ui   # 系统设置界面
├── studentinfowidget.h/.cpp/.ui # 学生信息管理模块
├── schedulewidget.h/.cpp/.ui   # 课程安排模块
├── financialwidget.h/.cpp/.ui  # 财务管理模块（含图表）
├── honorwallwidget.h/.cpp/.ui  # 荣誉墙模块
├── tabledelegates.h            # 自定义表格委托
├── src.qrc                     # Qt 资源文件（QSS、图片）
├── style/                      # QSS 样式表目录
│   ├── color_palette.qss       # 颜色变量定义
│   ├── main_style.qss          # 主窗口样式
│   ├── nav_style.qss           # 导航栏样式
│   └── widget_style.qss        # 通用组件样式
└── data/                       # 数据库文件目录
    └── student_system.db       # SQLite 数据库文件
```

### 核心类说明

| 类名 | 职责 | 设计模式 |
|------|------|----------|
| `DataBaseManager` | 数据库连接、初始化、CRUD 操作 | 单例模式 |
| `Settings` | 应用程序配置管理（INI 文件） | 单例模式 |
| `MainWindow` | 主窗口框架、导航切换 | - |
| `LoginDialog` | 用户认证、密码加密 | - |
| `StudentInfoWidget` | 学生信息管理 UI 与逻辑 | - |
| `FinancialWidget` | 财务管理 + 数据可视化 | - |
| `HonorWallWidget` | 荣誉墙图片管理 | - |
| `ClickableLabel` | 可点击的图片标签（继承自 QLabel） | - |

---

## 🎨 技术亮点

### 1. 单例模式实现

```cpp
// databasemanager.h
class DataBaseManager : public QObject
{
    Q_OBJECT
public:
    static DataBaseManager& instance() {
        static DataBaseManager inst;
        return inst;
    }
    
private:
    DataBaseManager() { /* 初始化数据库 */ }
    ~DataBaseManager() override;
};
```

### 2. 密码加密（SHA-256）

```cpp
// settings.cpp
QString Settings::encryptPassword(const QString& password) const
{
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(), 
        QCryptographicHash::Sha256
    );
    return hash.toHex();
}
```

### 3. 数据库操作封装

```cpp
// databasemanager.cpp
bool DataBaseManager::insertStudentInfo(
    const QString& id, const QString& name, /* ... */)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO studentInfo VALUES "
                  "(:id, :name, :gender, :birthday, ...)");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    // ...
    return query.exec();
}
```

### 4. Qt Charts 数据可视化

```cpp
// financialwidget.cpp
void FinancialWidget::updatePieChart()
{
    QPieSeries *series = new QPieSeries();
    series->append("学费", tuitionAmount);
    series->append("书本费", bookAmount);
    
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("缴费类型分布");
    
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}
```

### 5. 自定义委托（Custom Delegate）

```cpp
// tabledelegates.h
class ImageDelegate : public QStyledItemDelegate
{
    void paint(QPainter *painter, 
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        // 自定义绘制图片单元格
        QByteArray imageData = index.data().toByteArray();
        QPixmap pixmap;
        pixmap.loadFromData(imageData);
        painter->drawPixmap(option.rect, pixmap);
    }
};
```

---

## 📝 许可证

本项目采用 **MIT 许可证** 开源。

```
Copyright (c) 2024 Education System

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 🤝 贡献指南

欢迎贡献代码、报告 Bug 或提出新功能建议！

### 贡献流程

1. **Fork 本仓库**
2. **创建特性分支**
   ```bash
   git checkout -b feature/AmazingFeature
   ```
3. **提交更改**
   ```bash
   git commit -m 'Add some AmazingFeature'
   ```
4. **推送到分支**
   ```bash
   git push origin feature/AmazingFeature
   ```
5. **创建 Pull Request**

### 代码规范

- 遵循 **Qt C++ 编码规范**
- 使用 **C++17** 标准
- 注释使用 **Doxygen** 格式
- 提交信息遵循 [Conventional Commits](https://www.conventionalcommits.org/)

### 报告 Bug

请通过 Issues 报告 Bug，并提供以下信息：

1. 操作系统和 Qt 版本
2. 复现步骤
3. 预期行为和实际行为
4. 截图或错误日志（如有）

### 功能建议

欢迎提出新功能建议，请说明：

1. 功能描述
2. 使用场景
3. 预期效果

---

## 📞 联系方式


- **邮箱**: [3614644417@qq.com]

---

## 🙏 致谢

感谢以下开源项目：

- [Qt Framework](https://www.qt.io/) - 跨平台开发框架
- [SQLite](https://www.sqlite.org/) - 嵌入式数据库
- [Qt Charts](https://doc.qt.io/qt-6/qtcharts-index.html) - 图表库

---

<div align="center">

**如果这个项目对你有帮助，请给一个 ⭐️ Star！**

Made with ❤️ by Education System Team

</div>
