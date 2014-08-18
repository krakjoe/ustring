--TEST--
Test UString::contains
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";
$string = new UString($std);

var_dump($string->contains("ちは"));
var_dump($string->contains(new UString("ちは")));
?>
--EXPECTF--
bool(true)
bool(true)



