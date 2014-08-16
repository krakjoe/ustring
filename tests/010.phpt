--TEST--
Test UString::truncate
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

var_dump($string->length());
$string->truncate(2);
var_dump($string->length());
?>
--EXPECTF--
int(7)
int(2)
