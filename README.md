ustring
=======

The following Unicode String support for PHP7 ...

```php
<?php
abstract class UString {
    public function __construct([string $string, string $codepage [, int $length]]);
    
    /** 
    * @return int
    **/
    public function length();
    
    /** 
    * @return void
    **/
    public function toUpper(void);
    
    /** 
    * @return void
    **/
    public function toLower(void);
    
    /** 
    * @return void
    **/
    public function reverse(void);
    
    /**
    * @return int new length
    **/
    public function trim(void);
    
    /** 
    * @return int new length
    **/
    public function truncate(int $length);
    
    /** 
    * @return int new length
    **/
    public function append(UString $string);
    
    /** 
    * @return int new length
    **/
    public function replace(UString $string, UString $replace);
    
    /**
    * @return bool
    **/
    public function startsWith(UString $needle [, int $start [, int $length]]);
    
    /**
    * @return bool
    **/
    public function endsWith(UString $needle [, int $start [, int $length]]);
    
    /**
    * @return int
    **/
    public function indexOf(UString $needle [, int $start [, int $length]]);
    
    /**
    * @return int
    **/
    public function lastIndexOf(UString $needle [, int $start [, int $length]]);
    
    /**
    * @return UString
    **/
    public function charAt(int $index);
}
?>
```

It's actually simple ...

*Note: UString is interchangable with zend strings for method parameters and can be cast for output/conversion to zend strings*
