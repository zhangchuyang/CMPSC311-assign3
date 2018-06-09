fileName=`ls *.$1`
n=0
h=00
h1=0
if [ $# -gt 2 ];
then
echo "illegal number of parameters"
exit 0
fi

for file in $fileName
do
n=`expr $n + 1`
if [ $n -lt 10 -a $n -gt 0 ];
then
mv $file "$2$h$n.$1"
elif [ $n -lt 100 -a $n -gt 10 ];
then
mv $file "$2$h1$n.$1"
else
mv $file "$2$n.$1"
fi 
done

