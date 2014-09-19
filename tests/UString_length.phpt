--TEST--
Test UString::$length
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

var_dump($string->length);
var_dump(strlen($string));
$string->length = 3;
?>
--EXPECTF--
int(7)
int(21)

Fatal error: Writing to UString properties is unsupported in %s on line %d