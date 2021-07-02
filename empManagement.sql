UNLOCK TABLES;

DROP DATABASE IF EXISTS `employees`;

CREATE DATABASE IF NOT EXISTS `employees`;
USE `employees`;

DROP TABLE IF EXISTS `user`;

CREATE TABLE `user` (
  `username` varchar(60) NOT NULL,
  `fname` varchar(45) DEFAULT NULL,
  `lname` varchar(45) DEFAULT NULL,
  `address` varchar(255) DEFAULT NULL,
  `password` varchar(32) NOT NULL,
  `jobTitle` varchar(60) NOT NULL,
  `socialSecurityNum` varchar(10) NOT NULL,
  `phoneNumber` varchar(12) NOT NULL,
  `email` varchar(150) NOT NULL,
  `sex` varchar(7) NOT NULL,
  PRIMARY KEY (`username`),
  UNIQUE KEY `username_UNIQUE` (`username`)
);


--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES ('dd','dd','dd','ddd','password', 'jobTitle', 'socialSecurityNum', 'phoneNumber', 'email', 'sex'),('dd1','qqw','qqw','qwq','0a93c4e8557cb6897b438861e21c55ee', 'jobTitle', 'socialSecurityNum', 'phoneNumber', 'email', 'sex'),('dhruv','dhruv','patel','fullerton','1aabac6d068eef6a7bad3fdf50a05cc8', 'jobTitle', 'socialSecurityNum', 'phoneNumber', 'email', 'sex'),('dhruv123','dhruv','patel','Ca','7199101025e18e6f160d764a7ca71180', 'jobTitle', 'socialSecurityNum', 'phoneNumber', 'email', 'sex'),('dhruvtest','dhruv','patel','Ca','7199101025e18e6f160d764a7ca71180', 'jobTitle', 'socialSecurityNum', 'phoneNumber', 'email', 'sex');
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `useraccount`
--

DROP TABLE IF EXISTS `useraccount`;

CREATE TABLE `useraccount` (
  `username` varchar(60) NOT NULL,
  `accountname` varchar(125) NOT NULL,
  `accountnumber` decimal(16,0) NOT NULL,
  `amount` decimal(20,2) unsigned NOT NULL DEFAULT '0.00',
  PRIMARY KEY (`username`,`accountnumber`),
  CONSTRAINT `username` FOREIGN KEY (`username`) REFERENCES `user` (`username`)
) ENGINE=InnoDB;

--
-- Dumping data for table `useraccount`
--

LOCK TABLES `useraccount` WRITE;
/*!40000 ALTER TABLE `useraccount` DISABLE KEYS */;
INSERT INTO `useraccount` VALUES ('dd','what',2978574356471002,0.00),('dhruv','ok',1212121212121212,10099.00),('dhruv','dhruv',1234567887654321,173.00),('dhruv','new',1452740660675522,0.00);
/*!40000 ALTER TABLE `useraccount` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping events for database 'employees'
--

--
-- Dumping routines for database 'employees'