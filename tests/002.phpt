--TEST--
Test UString::startsWith
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std, "UTF-8");

var_dump($string->startsWith("こ"));
var_dump($string->startsWith(new UString("こ", "UTF-8")));
?>
--EXPECT--
bool(true)
bool(true)
