--TEST--
Test UString::replace
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界 歓迎";

$string = new UString($std);

$string->replace("歓迎", "welcome :)");

var_dump((string) $string);
?>
--EXPECTF--
string(18) "%s welcome :)"
