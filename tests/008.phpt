--TEST--
Test UString::reverse
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std, "UTF8");

$string->reverse();

var_dump((string) $string);
?>
--EXPECTF--
string(7) "%s"
