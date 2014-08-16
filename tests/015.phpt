--TEST--
Test UString concat operator
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";

$string = new UString($std, "UTF-8");

$result = $string .= ", whatever";

var_dump((string) $result);
?>
--EXPECTF--
string(17) "%s, whatever"

