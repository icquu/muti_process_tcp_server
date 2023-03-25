#!/bin/bash
DUMP=/usr/local/mongo/bin/mongodump
#备份文件临时目录
OUT_DIR=/databak/mongodb/tmp
#备份文件正式目录
TAR_DIR=/databak/mongodb
#备份文件将以备份时间保存
DATE=`date +%Y_%m_%d_%H_%M_%S`
#数据库操作员
DB_USER=hjgame
#密码
DB_PASS=hj888
#保留最新4天的备份
DAYS=4
#备份文件命名格式
TAR_BAK="mongodb_game_bak_$DATE.tar.gz"
#创建文件夹
cd $OUT_DIR
#清空目录
rm -rf $OUT_DIR/*
#rm -rf $TAR_DIR/*
#创建本次备份的文件夹
mkdir -p $OUT_DIR/$DATE
#执行备份命令
$DUMP --host 192.168.1.34 --port 3017 --authenticationDatabase=admin -u $DB_USER -p $DB_PASS -o $OUT_DIR/$DATE -d hjgame  --gzip
#将备份文件打包放入正式目录
tar -zcvf $TAR_DIR/$TAR_BAK $OUT_DIR/$DATE --remove-files
#删除4天前的旧备份
find $TAR_DIR/ -mtime +$DAYS -delete
#删除15天前的MySQL旧备份
find /databak/mysql/ -mtime +15 -delete
