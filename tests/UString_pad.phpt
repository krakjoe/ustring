--TEST--
Test UString::pad
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$std = new UString("こんにちは世界");

function print_str($ustr)
{
	echo "[$ustr]\n";
}

print_str($std->pad(7));
print_str($std->pad(9));
print_str($std->pad(9, '-', STR_PAD_LEFT));
print_str($std->pad(9, '-', STR_PAD_BOTH));

print_str($std->pad(11, '=-'));
print_str($std->pad(11, '=-', STR_PAD_BOTH));

print_str($std->pad(17, $std));

print_str($std->pad(13, '、');

?>
--EXPECTF--
[こんにちは世界]
[こんにちは世界  ]
[--こんにちは世界]
[-こんにちは世界-]
[こんにちは世界=-=-]
[=-こんにちは世界=-]
[こんにちは世界こんにちは世界こんに]
[こんにちは世界、、、、、、]