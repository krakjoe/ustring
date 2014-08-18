--TEST--
Test UString::replace
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界 歓迎";

$string = new UString($std);

var_dump($string->length());

$replaced = $string->replace("歓迎", "welcome :)");

var_dump($replaced->length());
?>
--EXPECT--
int(10)
int(18)
