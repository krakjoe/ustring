--TEST--
Test UString::endsWith
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

var_dump($string->endsWith("界"));
var_dump($string->endsWith(new UString("界")));
?>
--EXPECT--
bool(true)
bool(true)
