const readline = require('readline-sync');

function VendingMachine(items, prices) {
	
	this.items = [...items]; // Construct a deep copy of the array of items in the vending machine.
	this.prices = [...prices]; // Construct a deep copy of the array of prices in the vending machine.

	// Function prototypes... not sure these are done correctly.
	VendingMachine.prototype.showMenu = function() {
		
		console.log("Welcome to our flawless and secure vending experience!");

		for(let i = 0; i < 2; ++i) // var was changed to let.
		{
			console.log("Woot!");
		}
		
		console.log("\nOptions\n" + "--------");
		
		for(let i = 0; i < this.items.length; ++i) // var was changed to let, and i was re-initialized to 0 to properly print out all item options.
		{			
			console.log(i + ". " + this.items[i] +  " [$" + this.prices[i] + "]");
		}
	}

	VendingMachine.prototype.itemRequest = function() {
		
		this.showMenu();

		let itemNum = readline.question("\nPlease enter the item number: ");

		while(true) {				
			if ( itemNum.match(/^[0-4]$/) === null) { // Only allow 0, 1, 2, 3, 4.
				console.log("NOT A VALID OPTION. Try again");
				itemNum = readline.question("\nPlease enter the item number: ");
				}
			else
				break;
		}

		console.log("You selected item: ", itemNum);

		let numItemsStr = readline.question("How many " + this.items[itemNum] + "(s) would you like to purchase? ");
		let numItems = parseInt(numItemsStr);

		while(true) {

			if (numItemsStr % 1 === 0 && numItemsStr >= 1) { // Used to check if numItemsStr is a whole & positive number.
				numItems = parseInt(numItemsStr);
				console.log("You requested: ", numItems, " ", this.items[itemNum].toLowerCase(), "(s) which costs " + ( ((this.prices[itemNum] * 100) * numItems) / 100 ) );
				break;
			}
			else
				console.log("NOT A VALID QUANTITY. Try again\n");
				numItemsStr = readline.question("How many " + this.items[itemNum] + "(s) would you like to purchase? ");
			}

		this.inputCoins(this.items[itemNum], this.prices[itemNum], numItems);
	}

	VendingMachine.prototype.inputCoins = function(item, price, numItems) {

		let coinSum = 0.0; // Changed from var to let.
		
		while(true) {

			let insertedCents = readline.question("\nPlease insert the bills (whole numbers) and/or cents (e.g. .25) to purchase your "
			+  item.toLowerCase()+  " (" +  (coinSum)/100 + " inserted so far)\n Enter [r] to return your money. \n Enter [v] to start vending with the current amount."
			 + "\n Please enter the amount:  "); 
			
			let floatCents = parseFloat(insertedCents * 100);

			// This is where we implement big.js to fix % issues.
			const bigjs = require("big.js");
			let insertedCents2 = insertedCents;

			if(!insertedCents.match(/[a-qs-uw-z]/i) && insertedCents >= 0.01) { // Checks that no symbols, alphabet letters aside from r or v, or number smaller
																				// than a penny will be accepted as valid inputs.
				insertedCents = new bigjs(insertedCents2);

				if ( ( insertedCents.times(100) ).mod(1) != 0) { // Will not add a number to coinSum that has a decimal/remainder beyond thousandths place decimal.
					console.log("NOT A VALID INPUT. Try again");
				}
				else {
					floatCents = parseFloat(insertedCents.times(100)); // Converts to whole number.
					coinSum += floatCents;
				}
			}
			else {
				if(insertedCents === "r") {
					if (coinSum === 0) {
						console.log("Nothing to return.\n");
						return false;
					}
					else {
						this.returnMoney(coinSum);
						return false;
					}
				}

				if(insertedCents === "v") {
					if( ( coinSum / 100) >= price * numItems) {
						console.log("\nPlease claim your ", numItems, " ", item , "(s).  Also, returning to you the sum of $", 
						( ( coinSum - ( (price * numItems) * 100 ) ) / 100) + "\n");
						return false;
					}
					else {
						console.log("Sorry, insufficient funds!");
						this.returnMoney(coinSum);
						return false;
					}
				}
				else
					console.log("NOT A VALID INPUT. Try again");
			}
		}
	}

	VendingMachine.prototype.returnMoney = function(amount) {

		let dollars = 0, quarters = 0, dimes = 0, nickels = 0, pennies = 0;

		dollars = Math.floor( amount / 100 );
		amount %= 100;

		quarters = Math.floor( amount / 25 );
		amount %= 25;

		dimes = Math.floor( amount / 10 );
		amount %= 10;

		nickels = Math.floor( amount / 5 );
		amount %= 5;

		pennies = Math.floor( amount / 1 );
		amount %= 1;

		console.log("\nReturning a sum of $", ( ( dollars * 100 + quarters * 25 + dimes * 10 + nickels * 5 + pennies ) / 100 ) );
		console.log((dollars), " dollars");
		console.log((quarters), " quarters");
		console.log((dimes), " dimes");
		console.log((nickels), " nickels");
		console.log((pennies), " pennies");
	}

	VendingMachine.prototype.turnOn = function() {
		this.itemRequest();
	}
}

let vm = new VendingMachine(["Water", "Soda", "Pizza", "Taco", "Tesla"], [.50, .99, 1.99, 3.99, 850000.00]);

vm.turnOn();