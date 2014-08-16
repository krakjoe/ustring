--TEST--
Test UString::trim
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = " こんにちは世界 ";

$string = new UString($std, "UTF8");

var_dump($string->length());
$string->trim();
var_dump($string->length());
?>
--EXPECTF--
int(9)
int(7)
