--TEST--
Test UString::toUpper
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "abcdefg";

$string = new UString($std);

var_dump((string) $string->toUpper(),   
         (string) $string);
?>
--EXPECT--
string(7) "ABCDEFG"
string(7) "abcdefg"

