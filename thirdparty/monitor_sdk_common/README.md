# 提交规范

## 开发模式
blade-base-common和blade-base-thirdparty采用主干发布，分支开发的模式。

代码管理和提交方式通过**Merge Request**模式

## 命名规范
| 分支  | 命名  | 说明 |
|------------- |---------------| -------------|
| 主分支        | master | 此分为生产环境最新版本的代码，仅有主程序员有merge和push权限|
| 功能分支      | feature/*        | 新功能分支 |
| 补丁分支     |     hotfix/*       | BUG FIX分支  |

### 主分支 master
Git 默认唯一的主分支 master ，自动建立，被保护，不允许在其上提交代码，只能通过 Merge Request 将其他分支代码合并到主分支。

### 功能分支 feature
命名规则：feature/name-of-this-feature

日期: 分支开始日期 yyMMdd

功能名: -号分割的英文单词或数字，只允许小写字母

### 补丁分支 hotfix
命名规则：hotfix/name-of-this-bug 或者 hotfix/issue-103

功能名: -号分割的英文单词或数字，只允许小写字母
当使用 issue 命名时，需要指明 issue  编号


## 提交步骤

### 1 从master创建分支
```bash
git checkout -b $name-of-branch 
`````

### 2 更新代码提交分支
```bash
git add --all
git commit -am 'message'
`````

### 3 上传代码
```bash
git push origin $name-of-branch 
`````

### 4 创建merge request

方案一：通过上一部的代码提交，在提交日志中访问merge request创建地址

方案二：通过gitlab界面中的merge request发起创建

common库和thirdparty库的merge request指派人为 李斌（11091599）
