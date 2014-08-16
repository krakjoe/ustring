--TEST--
Test UString concat operator
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std);

var_dump($string->length());
$result = $string .= ", whatever";
var_dump($result->length());
?>
--EXPECT--
int(7)
int(17)
