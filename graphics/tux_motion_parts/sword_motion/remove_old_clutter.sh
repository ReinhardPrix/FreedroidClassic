
for i in $(find . -name "*.png"); do
    echo "File:" $i
    rm $i
done

for i in $(find . -name "*.offset"); do
    echo "File:" $i
    rm $i
done

