count=$#
total=0

for number in "$@"; do
    total=$((total + number))
done

average=$(echo "$total / $count" | bc)

echo "Количество аргументов: $count"
echo "Среднее арифметическое: $average"