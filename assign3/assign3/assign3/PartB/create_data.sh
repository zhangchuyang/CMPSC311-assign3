if [ -e graph.data ];
then
echo "Do you want delete the old graph.txt and create a new one? [y or n]"
read answer
        if [ $answer == "y" ];
        then
        rm graph.data
        elif [ $answer == "n" ];
        then
        echo "Then aborting"
        fi
fi



if [ $1 -eq 0 -o $2 -eq 0 ];
then
echo "Args can't be 0."
elif [ $1 -lt $2 ];
then
echo "Number of edge ($2) can't exceed number of node ($1)."
else
for ((i = 0; i < $1; i++))
do
for ((j = 0; j < $2; j++ ))
do
echo "$i $((0 + RANDOM % $2))" >> graph.data
done
done
fi

