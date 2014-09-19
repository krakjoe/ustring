--TEST--
Test UString::trim
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = " こんにちは世界 ";

$string = new UString($std);

var_dump($string->length);

$trimmed = $string->trim();

var_dump($trimmed->length);
?>
--EXPECTF--
int(9)
int(7)
