--TEST--
Test UString::chunk
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";
$string = new UString($std);

var_dump(count($string->chunk(1)));
?>
--EXPECTF--
int(7)



