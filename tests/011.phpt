--TEST--
Test UString::append
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std, "UTF-8");

var_dump($string->append(" 歓迎"));
?>
--EXPECT--
int(10)
