--TEST--
Test UString::length
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std, "UTF-8");

var_dump($string->length());
var_dump(strlen($string));
?>
--EXPECT--
int(7)
int(7)
