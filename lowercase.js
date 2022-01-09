const fs = require('fs');

let norvig = fs.readFileSync("norvigclean.txt").toString().split("\n");

let writer = fs.createWriteStream("norvigclean.txt", {
	mode: 0o755
});

let single_letters = [];

for (let lowercaser = 0; lowercaser < norvig.length; lowercaser++) {
	norvig[lowercaser] = norvig[lowercaser].split("\t");
	if (norvig[lowercaser][0].length == 1)
		single_letters.push([norvig[lowercaser][0], norvig[lowercaser][1]]);

	if (norvig[lowercaser][0].length > 2)
		writer.write(`${norvig[lowercaser][0].toLowerCase()} ${norvig[lowercaser][1]}\n`);
}

function quicksort(array, low, high) {
	if (low >= high)
		return;

	let pivot = low - 1;

	for (let j = low; j < high; j++) {
		if (array[j][0] < array[high][0]) {
			pivot++;
			let buffer = array[j];
			array[j] = array[pivot];
			array[pivot] = buffer;
		}
	}

	pivot++;
	let buffer = array[high];
	array[high] = array[pivot];
	array[pivot] = buffer;

	quicksort(array, pivot + 1, high);
	quicksort(array, low, pivot - 1);
}

quicksort(single_letters, 0, single_letters.length - 1);
console.log(single_letters);