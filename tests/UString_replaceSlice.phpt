--TEST--
Test UString::replaceSlice
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";
$string = new UString($std);

$replaced = $string->replaceSlice("hello", 1, 1);

var_dump((string) $replaced);
?>
--EXPECTF--
string(23) "%shello%s"


