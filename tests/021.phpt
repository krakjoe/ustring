--TEST--
Test UString::insert
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";
$string = new UString($std);

$string->insert(1, "hello");

var_dump((string) $string);
?>
--EXPECTF--
string(26) "%shello%s"

