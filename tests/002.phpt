--TEST--
Test UString::startsWith
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

var_dump($string->startsWith("こ"));
var_dump($string->startsWith(new UString("こ")));
?>
--EXPECT--
bool(true)
bool(true)
