--TEST--
Test UString read dimension
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";
$string = new UString($std);

for ($i = 0; $i < $string->length(); $i++) {
    var_dump(strlen($string[$i]));    
}
?>
--EXPECT--
int(3)
int(3)
int(3)
int(3)
int(3)
int(3)
int(3)

