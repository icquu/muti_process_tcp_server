acme.sh
############################################################################
$ sudo crontab -l -u acme
# use /bin/sh to run commands, overriding the default set by cron
SHELL=/bin/sh
# mail any output to here, no matter whose crontab this is
MAILTO=icquu@shark.com

7 22 * * * /usr/local/sbin/acme.sh --cron --home /var/db/acme/.acme.sh > /dev/null
############################################################################

/usr/local/sbin/acme.sh

李丽芬 爱不释手 
心湖雨又风
爱江上更爱美人
彭箏-太想念
爱我的人和我爱的人
蓝蓝白云天，悠悠水边柳，一曲《美人吟》
情火
再回首
一生所爱
得意的笑
凉凉
我无奈于这个世界,我可能没有能力去改变世界,哪怕很小的一个世界,我只能很努力地去做到世界无奈于我,尽量不被世界的事物所左右

大部分人惨,自然会帮助我们,一个人惨才是真的惨

升级mongodb:
db.adminCommand( { getParameter: 1, featureCompatibilityVersion: 1 } )
使用可升级版本,执行:
db.adminCommand( { setFeatureCompatibilityVersion: "4.4" } ) 

ssh -C -f -N -g -L 3307:172.16.41.20:3306 root@173.248.235.146 -p 22

autossh -M 5555 -NR 3307:172.16.41.20:3306 root@173.248.235.146 -p 22

change master to master_host='172.31.0.44', master_password='s4kw42Hb7R4!!qEd', master_user='dbmis', master_auto_position=1;


change master to master_host='127.0.0.1', master_password='s4kw42Hb7R4!!qEd', master_user='dbmis', master_port=3307 ,master_auto_position=1;

while [[ true ]]; do
	#statements
done