--TEST--
Test UString assign concat operator
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std, "UTF8");

$string .= ", whatever";

var_dump((string) $string);
?>
--EXPECTF--
string(17) "%s, whatever"

