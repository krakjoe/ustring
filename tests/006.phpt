--TEST--
Test UString::toLower
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std, "UTF-8");

var_dump($string->lastIndexOf("界"));
var_dump($string->lastIndexOf(new UString("界", "UTF-8")));
?>
--EXPECT--
int(6)
int(6)
