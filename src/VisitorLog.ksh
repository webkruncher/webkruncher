  



for notme in `cat /var/log/messages | grep -e "|GET|" | grep "|/Home\.xml"  | grep "40.77.189.250"`; do
OFS=$IFS
IFS=$'\n'
	for interesting in `cat /var/log/messages | grep -e "|GET|" |  grep "${notme}" | grep "|.*\.xml" | sed '/|\/index\./d' | sed '/|\/Home\./d' | sed '/|\/Left\./d'`; do
		whenwho=`echo -ne "${interesting}" | cut -d'|' -f 1`
		what=`echo -ne "${interesting}" | cut -d'|' -f 8`
		when=`echo -ne "${whenwho}" | cut -d ' ' -f1-3`
		
		[ ! -z `find /home/jmt/websites/text/webkruncher -name "${what:1}"` ] && color="\033[32m" || color="\033[34m" 
		echo -ne "${color}${when}|${what:1}\033[0m\n"
	done
IFS=$OFS
done

