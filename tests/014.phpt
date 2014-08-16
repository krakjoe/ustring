--TEST--
Test UString::charAt
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std, "UTF8");

var_dump((string)$string->charAt(1));
?>
--EXPECTF--
string(2) "%s"
