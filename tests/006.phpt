--TEST--
Test UString::toLower
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "ABCDEFG";

$string = new UString($std);

var_dump((string) $string->toLower(),   
         (string) $string);
?>
--EXPECT--
string(7) "abcdefg"
string(7) "ABCDEFG"

