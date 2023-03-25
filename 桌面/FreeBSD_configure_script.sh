#!/bin/sh
#FreeBSD折腾日记
#自动部署脚本与logo

echo "autoboot_delay=0"
kern.vty=vt" >> /boot/loader.conf

cd /usr/ports
make fetchindex
yes | pkg

pkg ins -y sudo vim wget tmux neofetch htop
if [ $? -ne 0 ]; then
	echo "pkg error 1"; exit
else
	pkg clean
fi

rm /usr/bin/vi /usr/share/man/man1/vi.1.gz
mv /usr/local/bin/vim /usr/bin/vi
mv /usr/local/man/man1/vim.1.gz /usr/share/man/man1/vi.1.gz
ln /usr/bin/vi /usr/local/bin/vim
ln /usr/share/man/man1//vi.1.gz /usr/local/man/man1/vim.1.gz
sed -i '' -e 's/# %wheel ALL=(ALL) NOPASSWD: ALL/%wheel ALL=(ALL) NOPASSWD: ALL/g' /usr/local/etc/sudoers

pkg ins -y kde5 sddm xorg drm-kmod wyq-fonts
if [ $? -ne 0 ]; then
	echo "pkg error 2"; exit
else
	pkg clean
fi
sysrc dbus_enable="YES"
sysrc sddm_enable="YES"
sysrc hald_enable="YES"

ls /home | xargs -I {} touch /home/{}/.Xauthority
ls /home | xargs -I {} chown {} /home/{}/.Xauthority
hostname | xargs -I {} sed -i '' -e 's/localhost localhost.my.domain/localhost localhost.my.domain {}.localdomain/g' /etc/hosts

cat > /etc/csh.cshrc <<"EOF"
# $FreeBSD$
#
# System-wide .cshrc file for csh(1).

alias h 	history 25
alias j		jobs -l
alias la 	ls --color=always -aF
alias lf 	ls --color=always -FA
alias ll 	ls --color=always -lAF
alias ls 	ls --color=always
alias sc 	screen -R
alias mv 	'mv -i'
alias cp 	'cp -i'
alias rm 	'rm -i'

bindkey '\e[1~' beginning-of-line	#Home
bindkey '\e[3~' delete-char			#Delete
bindkey '\e[4~' end-of-line			#End
bindkey '^W' backward-delete-word	#Delete
bindkey -k up history-search-backward	#PageUp
bindkey -k down history-search-forward	#PageDown

set nobeep
set noclobber
set autolist
set autoexpand

set cr = "%{\e[31m%]}"
set cg = "%{\e[32m%]}"
set c0 = "%{\e[0m%]}"

# Set some variables for interactive shells
if ( $?prompt ); then
	if ( "$uid" == "0" ); then
		set prompt = "%B%U%n%@%m.$cr%l$c0%b %c %B%#%b "
	else
		set prompt = "%B%U%n%@%m.$cr%l$c0%b %c %B%%%b "
	endif
endif

setenv GREP_OPTIONS --COLOR=auto
setenv EDITOR ee
setenv PAGER  less

if ( $?prompt ) then
	set prompt = "%N@%m:%~ %# "
	set promptchars = "%#"
	set filec
	set history = 100
	set savehist = (100 merge)
	set autolist = ambiguous
	set autorehash
	set mail = (/var/mail/$USER)
	if ( $?tcsh ) then
		bindkey '^W' backward-delete-word	#Delete
		bindkey -k up history-search-backward	#PageUp
		bindkey -k down history-search-forward	#PageDown
	endif
endif

#修改启动界面:
替换/boot/images中的几个文件:
freebsd-brand-rev.png
freebsd-brand.png
freebsd-log-rev.png