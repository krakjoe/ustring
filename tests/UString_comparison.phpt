--TEST--
Test UString comparison
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

var_dump($string == $std);
?>
--EXPECT--
bool(true)
