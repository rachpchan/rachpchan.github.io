"use strict";
// import express framework for routing
const express = require("express");
// The body parser
const bodyParser = require("body-parser");

// The mysql library
const mysql = require('mysql');
const session = require('express-session');
const xssFilters = require('xss-filters');
//const csp = require('helmet-csp');
// instantiate express
const app = express();
//database Connection
const  mysqlConn = mysql.createConnection({
	host: "localhost",
	user: "root",
	//password: "dhruv123",
  database:"employees",
	multipleStatements: true,
});
// check for valid name

const nameregex  = /^[a-zA-Z]{2,30}$/;
const usernameregex  = /^[a-zA-Z0-9]{2,30}$/;
const addressregex = /^[a-zA-Z0-9\s,'-]*$/;
mysqlConn.connect((err) => {
  if (err) throw err;
  console.log('Connected!');
});

//top part of html
function tophtml(username){
  let pagehtml="<!DOCTYPE html> <html lang='en'> <title> Welcome to employee magement application, </title> <body bgcolor='#ec7063'><div><h1>Welcome "+ username +" </h1></div><br>";
  return pagehtml
}
//bottom part of HTML
function bottomhtml(){
  let pagehtml="<a href=/home>Home</a></body> </html>";
  return pagehtml
}

function validateMoney(amount){
	amount = amount*100
		if(Number.isInteger(amount) && amount>=0 && amount!= Infinity){
			return true;
		}
		else{
			console.log("Not valid dollar amount. Check your dollar input")
			return false;
		}

}
function validateAccountnumber(accountnumber){
		if(Number.isInteger(accountnumber) && accountnumber>=0 && accountnumber.toString().length===16){
			return true;
		}
		else{
			console.log("Invalid Accoutnumber")
			return false;
		}

}
function GenrateAccountnumber(){
	let randomNumber=Math.random() * (9999999999999999-1000000000000000)+1000000000000000;
	return parseInt(randomNumber);
}

// needed to parse req body
app.use(express.urlencoded({ extended: true }));
/*
app.use(csp({
  directives: {
    defaultSrc: ["'self'"],
    scriptSrc: ["'self'"],
  }}))
*/

app.use(session({
	   cookieName: 'session',
	  duration: 30 * 60 * 1000,
	  activeDuration: 3 * 60 * 1000, // cookie will only be active for specified length
		secret: 'secret',
		resave: true,
		saveUninitialized: true,
	  cookie: {
	    maxAge: 300000, // duration of the cookie in milliseconds, defaults to duration above
	    httpOnly: true, // when true, cookie is not accessible from javascript
		}
}));

app.get("/", function (req, resp) {
	if (req.session.loggedin===true) {
			//console.log("here");
			console.log(req.session.username);
		 resp.redirect('/home');
	} else {
		//resp.sendFile(__dirname + "/login.html");
		resp.sendFile(__dirname + "/home.html");
	}
});

app.get("/emp_login", function (req, resp) {
	if(req.session.loggedin===true){
    	resp.sendFile(__dirname + "/index.html");
	}else{
		resp.sendFile(__dirname + "/emp_login.html");
	}
});

app.get("/manager_login", function (req, resp) {
	if(req.session.loggedin===true){
    	resp.sendFile(__dirname + "/index.html");
	}else{
		resp.sendFile(__dirname + "/manage_login.html");
	}
});

//	INSERT INTO `user` VALUES (req.session.username,req.session.username,'patel','fullerton','d');
app.get("/registration", function (req, resp) {

	if (req.session.loggedin===true) {
		resp.redirect('/home');
	}
	else {
		resp.sendFile(__dirname + "/registration.html");
	}

});

app.post("/registration", function (req, resp) {

	let username = xssFilters.inHTMLData(req.body.username);
	let fname = xssFilters.inHTMLData(req.body.lastName);
	let lname = xssFilters.inHTMLData(req.body.firstName);
	let address = xssFilters.inHTMLData(req.body.address);
	let password = xssFilters.inHTMLData(req.body.password);
	let jobTitle = xssFilters.inHTMLData(req.body.jobTitle);
	let socialSecurityNum = xssFilters.inHTMLData(req.body.socialSecurityNum);
	let phoneNumber = xssFilters.inHTMLData(req.body.phoneNumber);
	let email = xssFilters.inHTMLData(req.body.email);
	let sex = xssFilters.inHTMLData(req.body.sex);

	if(nameregex.test(fname ) && nameregex.test(lname) && usernameregex.test(username) && addressregex.test(address)){

		mysqlConn.query("SELECT `username` from `user` where `username`= ?",[username, fname, lname, address, password, jobTitle, socialSecurityNum, phoneNumber, email, sex],
		
		// Check if user is present or not.
		function(err, result) {

			if(err)
				throw err;
			console.log(result);

			if (result.length <= 0) {

				mysqlConn.query("INSERT INTO `user` VALUES (?,?,?,?,md5(?),?,?,?,?,?)",[username, fname, lname, address, password, jobTitle, socialSecurityNum, phoneNumber, email, sex],
				
				function(err, result) {

					if(err) throw err;
					let pagehtml= tophtml(username);
					pagehtml+="<div>Thank you, "+ xssFilters.inHTMLData(username)+" for registering. You may now login.<div>";
					pagehtml+="<a href='/'>Click here to login.<a></body> </html>";;
					resp.send(pagehtml);

				});

			} else{

				console.log(username);
				console.log("ERROR: Username is already present.");
				resp.sendFile(__dirname + "/Error.html");

			}
			});
	} else{
		console.log("Invalid inputs provided.");
		resp.sendFile(__dirname + "/Error.html");
	}



});

  app.get("/createaccount", function (req, resp) {

    if(req.session.loggedin===true){

    	resp.sendFile(__dirname + "/createaccount.html");
	}else{
		resp.sendFile(__dirname + "/login.html");
	}
	//resp.sendFile(__dirname + "/createaccount.html");
  });


  app.post("/createaccount", function (req, resp) {
    if (req.session.loggedin === true){

    let accountname = xssFilters.inHTMLData(req.body.accountname);
		let userSession =xssFilters.inHTMLData(req.session.username);
		let pagehtml=tophtml(userSession);
    // test the username
    if (nameregex.test(accountname)){
      let accountnumber=GenrateAccountnumber();
      mysqlConn.query("INSERT INTO `useraccount` VALUES (?,?,?,?)",[userSession,accountname,accountnumber,0],
      function(err, result){
    		if(err) throw err;
        console.log(result)
    	});
      pagehtml+="Employee account has been created. <br>";
      pagehtml+="Account name is " + xssFilters.inHTMLData(accountname) + "<br>"
      pagehtml+="Your employee ID number is <B>" + xssFilters.inHTMLData(accountnumber) +"</B><br>";
      pagehtml+= bottomhtml();

      resp.send(pagehtml);
    }else{
			console.log("Invalid account name.");
      resp.sendFile(__dirname + "/Error.html");
  }
}
  else{
	resp.sendFile(__dirname + "/login.html");
  }
  });


  app.get("/viewaccount", function (req, resp) {

    if (req.session.loggedin ===true){
			let userSession =xssFilters.inHTMLData(req.session.username);
			let pagehtml="";
			pagehtml=tophtml(userSession);
			mysqlConn.query("SELECT `accountname`,`accountnumber`, `amount` from `useraccount` where `username`= ?",[userSession],
			function(err, result){
				if(err) throw err;
				if(result.length>0){
					pagehtml+="<div>";
					for(let i=0;i<result.length;i++){
					//if account number not present
					pagehtml +="<div>";
				 pagehtml+="Employee Name: <b>" + xssFilters.inHTMLData(result[i].accountname) + "</b><br>";
				 pagehtml+="Employee ID: <b>" + xssFilters.inHTMLData(result[i].accountnumber) + "</b><br>";
 				 pagehtml+="BLANK_VALUE : <b>" + xssFilters.inHTMLData(result[i].amount) + "</b><br><br>";
				 pagehtml +="</div>";
				}
				pagehtml+="</div>";
 				 pagehtml+= bottomhtml();
 				 resp.send(pagehtml);
					//console.log("Account number  or username is not present");
				}
				else{
					//Account number  or username is not present
					pagehtml+=" Please create an account.";
					pagehtml+= "<br>" + bottomhtml();
  				 resp.send(pagehtml);
					// no account yet
				}
			});
		}
		else{
			resp.sendFile(__dirname + "/login.html");
		}

    //resp.sendFile(__dirname + "/registerfront.html");
  });

  app.get("/deposit", function (req, resp) {
	if(req.session.loggedin===true){
		resp.sendFile(__dirname + "/deposit.html");
		}else{
			resp.sendFile(__dirname + "/login.html");
		}
  });

  app.post("/deposit", function (req, resp) {
  if (req.session.loggedin ===true){
	let userSession =xssFilters.inHTMLData(req.session.username);
	let pagehtml="";
   pagehtml=tophtml(userSession);
  let accountnumber = parseInt(xssFilters.inHTMLData(req.body.accountnumber));
  let depositamount = parseFloat(xssFilters.inHTMLData(req.body.depositamount));

 	if(validateMoney(depositamount) && validateAccountnumber(accountnumber)){
		//	update bank.useraccount set amount=amount+5 where accountnumber='1212121212121212' and username=userSession;
		mysqlConn.query("UPDATE `useraccount` set `amount`=`amount`+? where `username`= ? and `accountnumber`= ?",[depositamount,userSession,accountnumber],
		function(err, result){
			if(err) throw err;
			if(result.changedRows===0){
				//if account number not present
				console.log("Account number  or username is not present");
			}
		});
		//update bank.useraccount set amount=amount+5 where accountnumber='1212121212121212' and username=userSession;
	mysqlConn.query("SELECT `username`,`accountnumber`,`amount` FROM `useraccount` where `username`= ? and `accountnumber`= ?",[userSession,accountnumber],
	function(err, result){
		if(err) throw err;
		// if accountnumber is present
		if (result.length > 0)
		{
		let outusername= xssFilters.inHTMLData(result[0].username);
		let outamount= xssFilters.inHTMLData(result[0].amount);
		let outaccountnumber= xssFilters.inHTMLData(result[0].accountnumber);
		pagehtml+= "<div> Hi "+ outusername + ", your balance for account number: <b>"+ outaccountnumber + "</b> is<b> " + outamount+"</b>.</div>";
		pagehtml+= bottomhtml();
	  console.log(pagehtml);
	  resp.send(pagehtml);
	}else{
		//if account number not present
		console.log("Account number is not present or invalid amount");
		resp.sendFile(__dirname + "/Error.html");
	}

	});

}else {
	console.log("invalid amount or account number");
	resp.sendFile(__dirname + "/Error.html");
}

}
  else{
	resp.sendFile(__dirname + "/login.html");
  }
});
  app.get("/withdraw", function (req, resp) {
	if(req.session.loggedin===true){
    	resp.sendFile(__dirname + "/withdraw.html");
	}
	else{
			resp.sendFile(__dirname + "/login.html");
    //resp.sendFile(__dirname + "/registerfront.html");
  	}
});


	app.post("/withdraw", function (req, resp) {

  if (req.session.loggedin ===true){
		let userSession =xssFilters.inHTMLData(req.session.username);
  let pagehtml=tophtml(userSession);

  let accountnumber = parseInt(xssFilters.inHTMLData(req.body.accountnumber));
  let withdrawamount = parseFloat(xssFilters.inHTMLData(req.body.withdrawamount));


 	if(validateMoney(withdrawamount) && validateAccountnumber(accountnumber)){
		//	update bank.useraccount set amount=amount+5 where accountnumber='1212121212121212' and username=userSession;

			mysqlConn.query("SELECT `amount` FROM `useraccount` where `username`= ? and `accountnumber`= ?",[userSession,accountnumber],
			function(err, result){
				if(err) throw err;
			if (result.length > 0)
			{
				// if accountnumber is present
			if (result[0].amount < withdrawamount){

				console.log("insufficient balance")
				resp.sendFile(__dirname + "/Error.html");
			}
		  else{
				//	update bank.useraccount set amount=amount-5 where accountnumber='1212121212121212' and username=req.session.username;
				mysqlConn.query("UPDATE `useraccount` set `amount`=`amount`-? where `username`= ? and `accountnumber`= ?",[withdrawamount,userSession,accountnumber],
			 function(err, result){
				 if(err) throw err;
			 });
			 mysqlConn.query("SELECT `username`,`accountnumber`,`amount` FROM `useraccount` where `username`= ? and `accountnumber`= ?",[userSession,accountnumber],
			 function(err, result){
				 if(err) throw err;
				 let outusername= xssFilters.inHTMLData(result[0].username);
				 let outamount= xssFilters.inHTMLData(result[0].amount);
				 let outaccountnumber= xssFilters.inHTMLData(result[0].accountnumber);
				 pagehtml+= "<div> Hi "+ outusername + ", your balance for account number: <b>"+ outaccountnumber + "</b> is<b> " + outamount+"</b>.</div>";

				 pagehtml+= bottomhtml();
				 resp.send(pagehtml);
			 });
		 }
	 }
		else{
				console.log("Entered account number is not present");
				resp.sendFile(__dirname + "/Error.html");
			}
});
}else {
	//if invalid amount or account number
	console.log("invalid amount or account number");
	resp.sendFile(__dirname + "/Error.html");
}

}
  else{
		// if session does not exist go back to login page
		resp.sendFile(__dirname + "/login.html");
  }
});
  app.get("/transfer", function (req, resp) {
    if(req.session.loggedin===true){
		resp.sendFile(__dirname + "/transfer.html");
		}else{
			resp.sendFile(__dirname + "/login.html");
		}
  });

	app.post("/transfer", function (req, resp) {

  if (req.session.loggedin ===true){
		let userSession =xssFilters.inHTMLData(req.session.username);
  let pagehtml=tophtml(userSession);
	let fromaccountnumber = parseInt(xssFilters.inHTMLData(req.body.fromaccountnumber));
  let toaccountnumber = parseInt(xssFilters.inHTMLData(req.body.toaccountnumber));
  let transferamount = parseFloat(xssFilters.inHTMLData(req.body.transferamount));

 	if(validateMoney(transferamount) && validateAccountnumber(fromaccountnumber)&& validateAccountnumber(toaccountnumber) &&(fromaccountnumber !== toaccountnumber)){
		//	update bank.useraccount set amount=amount+5 where accountnumber='1212121212121212' and username=userSession;
			//console.log(getBalance(userSession,accountnumber));
			mysqlConn.query("SELECT `amount` FROM `useraccount` where `username`= ? and `accountnumber`= ?",[userSession,fromaccountnumber],
			function(err, result){
				if(err) throw err;
				// if fromaccountnumber is present
			if (result.length > 0)
			{
			if (result[0].amount < transferamount){
				// if low balance
				console.log("insufficient balance or invalid account number")
				resp.sendFile(__dirname + "/Error.html");
			}
		  else{
				//	update bank.useraccount set amount=amount-5 where accountnumber='1212121212121212' and username=userSession;
				mysqlConn.query("UPDATE `useraccount` set `amount`=`amount`+? where `username`= ? and `accountnumber`= ?",[transferamount,userSession,toaccountnumber],
			 function(err, result){
				 if(err) throw err;
				 if(result.changedRows===0){
	 				//if toaccountnumber is not present do not add and withdraw money
	 				console.log("Account number or username is not present");
					resp.sendFile(__dirname + "/Error.html");
	 			}else{
			 mysqlConn.query("UPDATE `useraccount` set `amount`=`amount`-? where `username`= ? and `accountnumber`= ?",[transferamount,userSession,fromaccountnumber],
			function(err, result){
				if(err) throw err;
				pagehtml+= "<div> Hi "+ xssFilters.inHTMLData(req.session.username) + ", amount form account number: <b>"+ xssFilters.inHTMLData(fromaccountnumber)  + "</b> is transferred to account number <b> " + xssFilters.inHTMLData(toaccountnumber)+"</b>.</div>";
				pagehtml+="<a href='/viewaccount'>view account</a><br>";
				pagehtml+= bottomhtml();
				resp.send(pagehtml);
			});
		}
			});

		 }
	 }
		else{
			// transfer account is not present
				console.log("Entered account number is not present or invalid amount or invalid amount ");
				resp.sendFile(__dirname + "/Error.html");
			}
});
}else {
	//if invalid amount or account number
	console.log("invalid amount or account's number");
	resp.sendFile(__dirname + "/Error.html");
}

}
  else{
		// if session does not exist go back to login page
		resp.sendFile(__dirname + "/login.html");
  }
});
  app.get("/logout", function (req, resp) {
    console.log("logout");
    req.session.destroy();
		resp.redirect('/');

  });


	app.post('/auth', function(req, resp) {
    let username = xssFilters.inHTMLData(req.body.username);
    let password = xssFilters.inHTMLData(req.body.password);
		let userSession =xssFilters.inHTMLData(req.session.username);
    if (username && password) {

      mysqlConn.query('SELECT * FROM employees.user WHERE `username` = ? AND `password` = md5(?)', [username, password], function(error, results) {
        if (results.length > 0) {
          req.session.loggedin = true;
          req.session.username = username;
          resp.redirect('/home');
        } else { // error page

          //resp.send('Incorrect Username and/or Password!');
		  console.log("LOGIN ERROR");
          resp.sendFile(__dirname + "/error_login.html");

        }
      });
    } else { // error page
			let pagehtml="<!DOCTYPE html> <html lang='en'> <title> Welcome to employee mangement application! </title> <body bgcolor='#ec7063'>";
			pagehtml+="<div><h3>Please enter Username and Password!'</h3></div>";
			pagehtml+="<a href='/'></a></body></html>"
      resp.send(pagehtml);
    }
  });

	app.get("*", (req, resp) => {
		// error page
		resp.sendFile(__dirname + "/PAGE_404.html");
	});
app.listen(3000);