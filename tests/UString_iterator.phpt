--TEST--
Test UString iteration
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = "こんにちは世界";
$string = new UString($std);

foreach ($string as $character) {
    var_dump($character->length);
}

?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
