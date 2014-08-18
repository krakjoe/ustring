--TEST--
Test UString assign concat operator
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

var_dump($string->length());
$string .= ", whatever";
var_dump($string->length());
?>
--EXPECTF--
int(7)
int(17)

