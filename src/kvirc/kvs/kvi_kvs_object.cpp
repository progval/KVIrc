//=============================================================================
//
//   File : kvi_kvs_object.cpp
//   Created on Wed 08 Oct 2003 02:31:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_object.h"
#include "kvi_kvs_hash.h"
#include "kvi_kvs_kernel.h"
#include "kvi_window.h"
#include "kvi_app.h"

#include "kvi_modulemanager.h"
#include "kvi_console.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_out.h"
#include "kvi_mirccntrl.h"
#include "kvi_iconmanager.h"
#include "kvi_malloc.h"

#include "kvi_kvs_object_controller.h"
#include "kvi_kvs_object_functioncall.h"
#include "kvi_kvs_object_functionhandlerimpl.h"

#include <qmetaobject.h>
#include <qtimer.h>
#include <time.h>
#include <qiconset.h>



/*
	@doc: objects
	@title:
		Object scripting
	@short:
		Object scripting overview
	@keyterms:
		objects , object scripting , complex data structures
	@body:
		[big]Introduction[/big]

		The KVIrc scripting language is not object oriented in nature.
		Anyway , objects are a highlevel abstraction that allow
		to write complex code in a "clean" way.
		So I've added at least some pseudo-object support.[br][br]

		[big]Basic concepts[/big]

		Objects are arranged in tree structures.
		Each object that you create is either toplevel object or a children
		of another object. A toplevel object is a parentless one.
		Obviously all objects can have children objects.[br][br]

		When an object is destroyed , all its children are also destroyed.
		The toplevel objects are automatically destroyed when KVIrc quits.
		The objects are global to the entire application (this is different
		from previous KVIrc releases where the objects were local
		to the current frame window and arranged in a single tree
		with a builtin root object).[br][br]

		Each object is an instance of a class that defines its features.
		Each object has also a name , that is not necessary unique and is assigned
		by the programmer; the name is just a mnemonic expedient, and
		you may also not need it.[br][br]

		Each object is identified by an [b]OPAQUE UNIQUE ID[/b].
		The ID is assigned by KVIrc and can be held in any variable.
		You can think the object id as a "handle for the object" or the object's pointer.
		Any action performed on the object will require its ID.[br][br]

		[big]Creation and destruction[/big]

		To create an object you must use the [fnc]$new[/fnc]()
		function. [fnc]$new[/fnc]() requires three parameters:[br]
		- The object class (more about object classes later in this document)[br]
		- The ID of the parent object , (that can be 0 for toplevel objects).[br]
		- The object name (eventually empty)[br]
		[example]
			%myobject = [fnc]$new[/fnc]([class]object[/class],0,theName)
		[/example]
		[fnc]$new[/fnc]() returns the ID of the newly created object, or
		the STRING "0" if the object creation fails
		(it is a string because the object id's are generally strings, and 0 is an "invalid object id").
		In well written scripts it is not common that the object creation fails, anyway
		you can check if the creation has failed in the following way:[br]
		[example]
			[cmd]if[/cmd](%myobject)[cmd]echo[/cmd] "Object created!"
			else [cmd]echo[/cmd] "Object creation failed!"
		[/example]
		You can also test the object ID's for equality:[br]
		[example]
			[cmd]if[/cmd](%myobject == %anotherobject)[cmd]echo[/cmd] "This is the same object!";
		[/example]
		The parent object id is optional, if not specified it is assumed to be 0.
		The object name is optional , but it may help you later in finding the object.[br][br]

		To destroy an object use the [cmd]delete[/cmd] command. (In previous versions
		this command was named "destroy" and delete is currently aliased to that name too).[br]
		[example]
			[cmd]delete[/cmd] %myobject
		[/example]
		If the destroyed object has children objects , these are destroyed too.[br][br]

		[big]Fields : objects as pseudo-structures[/big]

		All the objects can contain variable fields.
		You can set an object's field by using the object scope operator "->":[br]
		[example]
			%myobject-&gt;%fieldVariable = dataString
		[/example]
		To unset a field set it with empty data string (just like with a normal variable).
		To retrieve the field data use the object scope operator in the same way:[br]
		[example]
			[cmd]echo[/cmd] %myobject->%fieldVariable
		[/example]
		The '-&gt;' operator has been stolen from the C language.
		In the KVIrc scripting language it switches from the global namespace
		to the object's one.[br]
		So in the above example %fieldVariable is owned by the object.[br]
		The first character of the variable name has no special meaning in the
		object namespace (in the global namespace the variables starting
		with an uppercase letter are global to the application , the other ones are local
		to the command sequence). The variable names are completely case insensitive.[br][br]

		Any [doc:operators]operator[/doc] can be used with the object field variables:[br]
		[example]
			%myobject-&gt;%fieldVariable = 0
			%myobject-&gt;%fieldVarialbe ++
			[cmd]if[/cmd]0(%myobject->%fieldVariable != 1)[cmd]echo[/cmd] KVIrc is drunk , maybe a reboot will help ?
		[/example]
		You can simulate C structures "on the fly" by using objects and fields:[br]
		[example]
			# Create an user description on the fly
			%myobj = [fnc]$new[/fnc]([class]object[/class],0,userDescription)
			# Set the fields
			%myobj-&gt;%nickname = Pragma
			%myobj-&gt;%username = daemon
			%myobj-&gt;%hostname = pippo.pragma.org
			%myobj-&gt;%info = Pragma goes always sleep too late
			%myobj-&gt;%info [doc:operators]&lt;&lt;[/doc] and wakes up too late too!
			# Call an (user defined) alias that stores the data to a file
			storetofile %myobj
			# Destroy the object
			[cmd]delete[/cmd] %myobj
		[/example]
		The field variables can be also dictionaries:[br]
		[example]
			%theobj-&gt;%field[key] = something
		[/example]
		Unlike in C , there is no need to declare object fields.
		Any object can have any field variable ; an "unset" field is equivalent to an "empty" field.[br]
		Note:[br]
		The KVIrc scripting language is not typed.
		Any object class (be patient...I'll explain classes in a while) identifier can be stored in any KVIrc variable:
		it is not possible to find out the object features by "examining" its identifier.
		This may make the usage of objects a bit "unclear";
		Howewer , with some experience you will be able to use the objects in a very powerful way.
		The type-safety can be also simulated by a careful usage of object names;
		in the above example , the %myobj object was created with the "userDescription" name.
		The storetofile alias could check the passed object's name and refuse to work
		if that does not match "userDescription".[br][br]

		A more complex use of fields will be described later in this document.[br][br]

		[big]Member functions[/big]

		Just like in C++ , the objects have member functions.
		For example , the "object" class (again...read on) objects export the [classfnc:object]$name[/classfnc]()
		and [classfnc:object]$className[/classfnc]() functions.[br]
		[example]
			%tmp = [fnc]$new[/fnc]([class]object[/class],0,myobject)
			[cmd]echo[/cmd] The object's name is %tmp->[classfnc:object]$name[/classfnc]() , the class name is %tmp->[classfnc:object]$className[/classfnc]()
			# Destroy the object
			[cmd]delete[/cmd] %tmp
		[/example]
		Another cool function exported by the [class:object]object[/class] class is the
		[classfnc:object]$children[/classfnc]() function.
		It returns a comma separated list of children identifiers.[br]
		[example]
			%tmp = [fnc]$new[/fnc]([class]object[/class],0,myobject)
			%tmpchild = [fnc]$new[/fnc]([class]object[/class],%tmp,child1)
			%tmpchild = [fnc]$new[/fnc]([class]object[/class],%tmp,child2)
			%tmpchild = [fnc]$new[/fnc]([class]object[/class],%tmp,child3)
			[cmd]echo[/cmd] The object's children list is : %tmp->[classfnc:object]$children[/classfnc]()
			# Destroy the object and the children
			[cmd]delete[/cmd] %tmp
		[/example]
		There are two special functions for each objects: the "constructor" and the "destructor".
		You will find more informations on constructors and destructors later in this document,
		for now it's enough that you know that these functions are called automatically by KVirc:
		the constructor is called when the object is created and the destructor is called when the
		object is being destroyed with [cmd]delete[/cmd].[br][br]

		The object functions can be reimplemented on-the-fly
		by using the [cmd]privateimpl[/cmd] command: you can simply modify the behaviour of the function
		by writing your own function body.
		(This is an uncommon feature: unlike many other languages , you can reimplement object
		functions at run-time, when the object has been already created.)[br][br]

		A more complex example[br]
		[example]
			%tmp = [fnc]$new[/fnc]([class]object[/class],0,myobject)
			[cmd]foreach[/cmd](%i,1,2,3)
			{
				%tmpchild = [fnc]$new[/fnc]([class]object[/class],%tmp,child%i)
				[cmd]privateimpl[/cmd](%tmpchild,destructor){ [cmd]echo[/cmd] Object [fnc]$this[/fnc] ([fnc]$this[/fnc]-&gt;[classfnc:object]$name[/classfnc]()) destroyed; }
			}
			[cmd]privateimpl[/cmd](%tmp,destructor)
			{
				%count = 0;
				[cmd]foreach[/cmd](%t,[fnc]$this[/fnc]-&gt;[classfnc:object]$children[/classfnc]())
				{
					[cmd]echo[/cmd] Children : %t-&gt;[classfnc:object]$name[/classfnc]() with class %t-&gt;[classfnc:object]$class[/classfnc]()
					%count++
				}
				[cmd]echo[/cmd] Just before destroying my %count children.
			}
			# Destroy the object and the children
			[cmd]delete[/cmd] %tmp
		[/example]

		In the example above four objects have been created.
		A "parent" object named "myobject" , and three children objects.
		The destructor has been reimplemented for each child object,
		to make it "say" its name (Please note the usage of [fnc]$this[/fnc]).
		In the parent destructor the children have been counted and listed.[br]
		Then the parent object is destroyed causing to:[br]
		- trigger the parent destructor.[br]
		- destroy all the children (and conseguently trigger all the "individual" destructors).[br][br]

		Not all the object functions must return a value:
		If a function does not return a meaningful value , or you just want to ignore it , you can call it in the following way:[br]
		[example]
			%anyobject-&gt;$functionname()
		[/example]
		[br]

		[big]Classes[/big]
		As said before , all objects are instances of a specific class.
		This concept is common to almost all object oriented languages.
		A class is a collection of methods that define an object's behaviour.
		Hehe...it is not easy to explain it , so I'll try with an example:[br]
		[example]
		class HostAddress
		{
			field hostname
			function ipnumber()
			function isLocalhost()
		}
		[/example]
		The above class is a rappresentation of a host address.
		You create an [b]instance of this class[/b] and set the hostname field, for example,
		to www.kernel.org.
		The object is now able to give you informations about the hostname in a transparent way:
		You can call the ipnumber() function, and the object will return you the
		digits and dots rappresentation of www.kernel.org.
		The isLocalhost() function will return true if the hostname refers to the local machine
		The object internal job is hidden from the user , but probably it will be a huge job.
		To obtain the IP number from the hostname , the object will probably have to perform a DNS call (usually a complex task).
		To check if the hostname references the local machine , the object will have to obtain the local hostname
		from the system (in some "unspecified" way) and then compare it with the given "hostname" field.[br][br]

		The internal job of the object is defined by the "implementation of the class".
		Obviously , the programmer that creates the class has to write that implementation.[br]

		[example]
		class HostAddress
		{
			field hostname
			function ipnumber()
			{
				find the nearest DNS server
				make the dns call
				wait for the response
				decode the response
			}
			function isLocalhost()
			{
				query the kernel for the local hostname
				compare the obtained hostname with the hostname field
			}
		}
		[/example]
		In the above example I have "implemented" the two functions by using a "fantastic" language.[br][br]

		Let's go back to the real world.[br][br]

		KVirc contains a [doc:classes]set of built-in ready-to-use classes[/doc].
		The basic class is [class]object[/class]: all the other classes are derived from this (more about
		object inheritance later in this doc).[br][br]

		Another available class is [class]socket[/class] that is an interface to the real system sockets.
		An instance of the [class]socket[/class] class can connect and communicate with other hosts on the net.[br][br]

		The [b]class definitions are GLOBAL to the entire application[/b]: all server windows share them.[br][br]

		So now we can say that in KVIrc
		[b]a CLASS is a collection of features that define the behaviour of an object.
		The user interface to the class are the member functions and the events.[/b][br][br]

		[big]Inheritance[/big]

		Someone asked for derived classes ?[br]
		Here we go:[br]
		The [cmd]class[/cmd] command allows you to define new object classes.
		In KVI++, A new class must be always derived from some other class: the lowest possible
		level of inheritance is 1: deriving from class [class]object[/class].[br]
		[example]
			[cmd]class[/cmd](helloworld,object)
			{
				sayhello()
				{
					[cmd]echo[/cmd] Hello world!
				}
			}
		[/example]

		The above class is named "helloworld". It inherits the [class]object[/class] class.
		This means that it acquires all the [class]object[/class] fuunctions: [classfnc:object]$name[/classfnc]() ,
		[classfnc:object]$class[/class]() , [classfnc:object]$children[/classfnc]()...
		Additionally , it has the $sayhello() function, that "echoes Hello world" to the console.
		Now you can create an instance of this class:
		[example]
			%instance = [fnc]$new[/fnc](helloworld)
			%instance->$sayhello()
		[/example]
		You should see "Hello world" printed in the console.
		Easy job...let's make the things a bit more complex now:
		derive another class from helloworld and make it say "hello" in two different languages:[br]
		[example]
		[cmd]class[/cmd](localizedhelloworld,helloworld)
		{
			[comment]# define the setlanguage function[/comment]
			[comment]# note that <$0 = language> is just a programmer reminder[/comment]
			setlanguage(<$0 = language>)
			{
				[cmd]if[/cmd](($0 == english) || ($0 == italian))
				{
					[fnc:$this]$$[/fnc]->%lang = $0
					[cmd]return[/cmd] 1
				} else {
					[cmd]echo[/cmd] I don't know that language ($0)
					[cmd]echo[/cmd] defaulting to english
					[fnc:$this]$$[/fnc]->%lang = english
					[cmd]return[/cmd] 0
				}
			}

			sayhello()
			{
				[cmd]if[/cmd]([fnc:$this]$$[/fnc]->%lang == italian)[cmd]echo[/cmd] Ciao mondo!
				else [fnc:$this]$$[/fnc]->$helloworld:sayhello()
			}
		}
		[/example]
		Now you can call:[br]
		[example]
		%m = [fnc]$new[/fnc](localizedhelloworld)
		%m->$setLanguage(italian)
		%m->$sayhello()
		%m->$setLanguage(english)
		%m->$sayhello()
		%m->$setLanguage(turkish)
		%m->$sayhello()
		[cmd]delete[/cmd] %myobj
		[/example]
		The class defined above is inherited from the previously defined helloworld class:
		so it inherits the "object" class functions and events and the sayhello function from "helloworld".
		In addition a setlanguage function is defined that stores in a variable the language name passed
		as a parameter (after checking its validity). ($0 evaluates to the first parameter passed)
		If the language is unknown the setlanguage function will return 0 (false).
		Now we want to be able to say "hello world" in italian and english.
		So we [b]override[/b] the inherited sayhello function.
		"To override" means "to reimplement" : if you call %object->$sayhello() and %object
		contains the ID of an instance of class "localizedhelloworld" , the new implementation of that function willl be called (executed).
		The inherited sayhello was able to say "hello world" only in english , so we can still use it in the new implementation
		without rewriting its contents. So if the language set is "not italian" we assume that it is english and
		call the [b]base class implementation[/b].[br]
		[example]
			[fnc]$this/[fnc]->$helloworld:sayhello()
			[comment]# equivalent to $$->$helloworld:sayhello(),[/comment]
			[comment]# to $this->$helloworld::sayhello(),[/comment]
			[comment]# and to $$->$helloworld::sayhello()[/comment]
		[/example]
		otherwise the language is italian and we say "hello" in italian :).
		So , to call a base class implementation of a function we "prepend" the base class name before the function name in the call.
		The base class name could be also [class]object[/class] in this case , but the [class]object[/class] class has no "sayhello" function defined
		so it would result in an error.[br][br]
		In the above example , all the values of [fnc]$this[/fnc]</a>-&gt;%language
		that are not equal to "italian" are assumed to be "english".
		This is not always true , for example , just after the object creation the %language variable field
		is effectively empty. The above class works correctly in this case , but we might want to have always
		a coherent state of the field variables , so we need another concept: the class [b]constructor[/b]
		that will be discussed in the next paragraph.[br][br]

		Note: multiple inheritance (inheritance from more than one base class) is not implemented , KVIrc is not a compiler :)[br][br]

		Objects are much more powerful....[br][br]

		Do a [cmd]clearobjects[/cmd] to cleanup the old class definitions , and read on.[br][br]

		[big]Constructors and destructors[/big]

		The class constructor is a [b]function[/b] that is called automatically just after the object
		has been created internally by KVIrc and just before the [fnc]$new[/fnc]
		function returns. It should be used to setup the internal object state.[br]
		Unlike in C++ , in KVIrc , the constructor CAN return a value:[br]
		If it returns 0 it signals the object creation failure : the object
		is immediately destroyed and [fnc]$new[/fnc]() returns 0 to the caller.
		Any other return value is treated as success , so the object is effectively
		created and [fnc]$new[/fnc]() returns its ID to the caller.[br]
		All the builtin classes have a constructor defined that will almost never fail (only if we run out of memory),
		so you can avoid to check the [fnc]$new[/fnc]() return value
		when creating the instances of the built-in classes.[br][br]

		In derived classes you can override the constructor to setup your object's state.[br]
		You should [b]always call the base class constructor[/b] in your overridden one , to setup
		the base class state , and propagate its return value (eventually modified if the base class
		constructor is succesfull but your derived class initialization fails).[br]
		In practice , the builtin class constructors do nothing other than setting the return
		value to 1 so you can even avoid to call them, but in any other case you must do it.[br][br]

		This is different from C (for example), where the constructors are called (more or less)
		automatically.[br][br]

		[big]Signals and slots[/big]

		The signals and slots are a powerful mean of inter-object communication.
		A signal is emitted by an object to notify a change in its state.
		For example , the [class:button]button class[/class] emits the
		[classsignal:button]clicked[/classsignal] signal when the user clicks the button.[br][br]
		A signal is emitted by an object and can be received and handled by any other existing object
		(including the object that emits the signal).[br]
		The handler function for a signal is called "slot".[br]
		It is just a convention : in fact , a slot is a normal object function (and any object function can be a slot).
		More than one slot can be connected to a single signal , and more signals can be connected to a single slot.[br]
		In this way , many objects can be notified of a change in a single object , as well as a single object
		can easily handle state-changes for many objects.[br]
		The signal / slot behaviour could be easily implemented by a careful usage of object functions.
		[b]So why signals and slots ?[/b][br]
		Because signals are much more powerful in many situations.
		The signals have no equivalent in C/C++...but they have been implemented in many highlevel
		C/C++ libraries and development kits (including the system-wide signal/handler mechanism implemented
		by all the modern kernels and used in inter-process communication).[br]
*/




///////////////////////////////////////////////////////////////////////////////////////

/*
	@doc: object
	@keyterms:
		object class, object, class
	@title:
		object class
	@type:
		class
	@short:
		Base class for all the KVIrc objects
	@inherits:
		none
	@description:
		This is the base class for all the builtin KVirc object classes.
		It exports the functions to retrieve the object name, to iterate
		through children objects and to lookup a child object by name or class.
		Additionally , this class provides the builtin timer functionality.
		The [classfnc]$constructor[/classfnc] and [classfnc]$destructor[/classfnc]
		functions are empty implementations that all the other classes inherit.
	@functions:
		!fn: $constructor()
		Constructor for this object class.
		The default implementation does nothing.
		!fn: $destructor()
		Destructor for this object class.
		The default implementation emits the signal "[classsignal]destroyed[/classsignal]".
		!fn: $name()
		Returns the name of this object.
		!fn: $parent()
		Returns the parent object of this object or 0 if this object has no parent.
		!fn: $timerEvent(<timerId>)
		Handler for the timer events.
		The default implementation does nothing.
		See also [classfnc]$startTimer[/classfnc]()
		and [classfnc]$killTimer[/classfnc]().
		!fn: $startTimer(<timeout>)
		Starts a builtin timer for this object and returns its timer id
		as a string or '-1' if the <timeout> was invalid.
		The [classfnc]$timerEvent[/classfnc]() handler function
		will be called at each <timeout>. The <timeout> is in milliseconds.
		!fn: $killTimer(<timer id>)
		Stops the timer specified by <timer id>.
		!fn: $killTimers()
		Stops all the builtin timers running.
		!fn: $className()
		Returns the class name of this object instance
		!fn: $findChild(<class>,<name>)
		Returns the first child that matches <class> and <name>.
		If <class> is an empty string, any class matches,
		if <name> is an empty string, any name matches.
		This function traverses the entire tree of children
		but is NOT recursive.
		!fn: $childCount()
		Returns the number of children objects
		!fn: $emit(<signal_name>[,parameters])
		Emits the signal <signal_name> passing the optional [parameters].
		See the [doc:objects]objects documentation[/doc] for an overview of signals and slots.
		!fn: $children()
		Returns an array of children object identifiers.
		!fn: $signalSender()
		Returns the current signal sender when in a slot connected to a signal.
		In other contexts this function returns an empty string.
		You can safely use it to test if the current function has been
		triggered directly or from a signal emission.
		!fn: $signalName()
		Returns the name of the signal last signal that has triggered
		one of this object's slots.
		This means that in a slot handler it returns the name of the signal
		that has triggered it.
		!fn: $property(<Qt property name>[,bNowarning:boolean])
		This is for really advanced scripting.[br]
		All KVIrc widgets are based on the Qt library ones.[br]
		The Qt library allow to set and read special properties.[br]
		You will have to take a look at the Qt documentation for each widget type
		to see the available property names.[br]
		The supported property types are: Rect, Size, Point, Color, String, CString,
		Int, UInt, Bool and enumeration types.[br]
		For example, the widget's x coordinate can be retrieved by using the [classfnc]$x[/classfnc]()
		function or by calling $property(x).[br]
		There are many properties that are available ony through the [classfnc]$property()[classfnc]" call:[br]
		For example, you can find out if the widget accepts drops by calling [classfnc]$property(acceptDrops)[classfnc].[br]
		This function will be mainly useful in the [class]wrapper[/class] class.
		!fn: $setProperty(<Qt property>,<property value>)
		Sets a qt property for this widget.[br]
		This is for advanced scripting, and can control really many features of the Qt widgets.[br]
		For example, the [class]multilineedit[/class] widgets can be set to
		the "password" echo mode only by using this function call:[br]
		[example]
			%X=$new(lineedit, 0, a_name)
			%X->$show()
			%X->$setProperty(echoMode,Password)
		[/example]
		The available properties to be set are listed by [classfnc]$listProperties[/classfnc]()[br]
		and must appear in the list as writeable.[br]
		This function will be mainly useful in the [class]wrapper[/class] class.
		!fn: $listProperties([bArray])
		Lists the properties of this object.[br]
		If <bArray> is $true then the function returns the properties
		as an array of descriptive strings, otherwise the properties are dumped to the
		active window. If <bArray> is not passed then it is assumed to be $false.
		This function will be mainly useful in the [class]wrapper[/class] class.
	@signals:
		!sg: destroyed()
		Emitted by the default implementation of [classfnc]$destructor[/classfnc].
		If you reimplement [classfnc]$destructor[/classfnc] in one of the derived
		classes (or as a private implementation), and still want this signal
		to be emitted you must emit it by yourself, or (better) call the base class
		destructor.
*/

// we use a char * pointer just to store a number
// we don't use void * just because incrementing a void pointer doesn't look that good
static char * g_hNextObjectHandle = (char *)0;


KviKvsObject::KviKvsObject(KviKvsObjectClass * pClass,KviKvsObject * pParent,const QString &szName)
: QObject(pParent)
{
	if(g_hNextObjectHandle == 0)g_hNextObjectHandle++; // make sure it's never 0
	m_hObject = (kvs_hobject_t)g_hNextObjectHandle;
	g_hNextObjectHandle++;

	m_pObject            = 0;
	m_bObjectOwner       = true; // true by default

	m_szName             = szName;

	m_pClass             = pClass;

	m_pChildList         = new KviPointerList<KviKvsObject>;
	m_pChildList->setAutoDelete(false);

	m_pDataContainer     = new KviKvsHash();

	m_pFunctionHandlers  = 0; // no local function handlers yet!

	m_bInDelayedDeath    = false;

	m_pSignalDict        = 0; // no signals connected to remote slots
	m_pConnectionList    = 0; // no local slots connected to remote signals

	if(pParent)pParent->registerChild(this);

	KviKvsKernel::instance()->objectController()->registerObject(this);

//	debug("Hello world!");
//	[root@localhost cvs3]# kvirc
//	Hello world!
//	[root@localhost cvs3]# date
//	Tue Sep  5 21:53:54 CEST 2000
//	[root@localhost cvs3]#

//  Ported to KVS on 29.04.2005
}

KviKvsObject::~KviKvsObject()
{
	m_bInDelayedDeath = true;

	callFunction(this,"destructor");

	while(m_pChildList->first())delete m_pChildList->first();
	delete m_pChildList;

#if 0
	// Disconnect all the signals
	if(m_pSignalDict)
	{
		KviPointerHashTableIterator<QString,KviKvsObjectConnectionList> it(*m_pSignalDict);

		while(it.current())
		{
			KviKvsObjectConnectionListIterator cit(*(it.current()));
			while(cit.current())
			{
				disconnectSignal(it.currentKey(),cit.current());
				// ++cit // NO!...we point to the next now!
			}
			// the iterator should automatically point to the next now
			//if(m_pSignalDict)++it;
		}
	}

	// Disconnect all the slots
	if(m_pConnectionList)
	{
		KviKvsObjectConnectionListIterator cit(*m_pConnectionList);
		while(cit.current())
		{
			QString szSig = cit.current()->szSignal;
			cit.current()->pSourceObject->disconnectSignal(szSig,cit.current());
			//++cit;// NO!... we point to the next now!
		}
	}
#else
	// Disconnect all the signals
	for(;;)
	{
		if(!m_pSignalDict)break;
		KviPointerHashTableEntry<QString,KviKvsObjectConnectionList> * pSignalList = m_pSignalDict->firstEntry();
		if(!pSignalList)break;
		KviKvsObjectConnection * pConnection = pSignalList->data()->first();
		if(!pConnection)break;
		disconnectSignal(pSignalList->key(),pConnection);
	}

	// Disconnect all the slots
	for(;;)
	{
		if(!m_pConnectionList)break;
		KviKvsObjectConnection * pConnection = m_pConnectionList->first();
		if(!pConnection)break;
		QString szSignalCopy = pConnection->szSignal; // we need this since pConnection is deleted inside disconnectSignal() and pConnection->szSignal dies too (but is referenced after the connection delete)
		pConnection->pSourceObject->disconnectSignal(szSignalCopy,pConnection);
	}
#endif

	KviKvsKernel::instance()->objectController()->unregisterObject(this);

	if(parentObject())parentObject()->unregisterChild(this);

	if(m_pObject)
	{
		disconnect(m_pObject,SIGNAL(destroyed()),this,SLOT(objectDestroyed()));
		if(m_bObjectOwner)delete m_pObject;
	}

	delete m_pDataContainer;
	if(m_pFunctionHandlers)delete m_pFunctionHandlers;
}

bool KviKvsObject::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	return true;
}

QWidget * KviKvsObject::parentScriptWidget()
{
	if(parentObject())
	{
		if(parentObject()->object())
		{
			if(parentObject()->object()->isWidgetType())
				return (QWidget *)(parentObject()->object());
		}
	}
	return 0;
}

void KviKvsObject::unregisterChild(KviKvsObject *pChild)
{
	m_pChildList->removeRef(pChild);
}

void KviKvsObject::registerChild(KviKvsObject *pChild)
{
	m_pChildList->append(pChild);
}

// SIGNAL/SLOT stuff

bool KviKvsObject::connectSignal(const QString &sigName,KviKvsObject * pTarget,const QString &slotName)
{
	if(!pTarget->lookupFunctionHandler(slotName))return false; // no such slot

	if(!m_pSignalDict)
	{
		m_pSignalDict = new KviPointerHashTable<QString,KviKvsObjectConnectionList>(7,false);
		m_pSignalDict->setAutoDelete(true);
	}

	KviKvsObjectConnectionList * l = m_pSignalDict->find(sigName);
	if(!l)
	{
		l = new KviKvsObjectConnectionList;
		l->setAutoDelete(true);
		m_pSignalDict->insert(sigName,l);
	}

	KviKvsObjectConnection * con = new KviKvsObjectConnection;
	debug("set connetion signal %s - slot %s",sigName.utf8().data(),slotName.utf8().data());

	con->pSourceObject = this;
	con->pTargetObject = pTarget;
	con->szSignal      = sigName;
	con->szSlot        = slotName;

	l->append(con);
	pTarget->registerConnection(con);
	return true;
}

void KviKvsObject::registerConnection(KviKvsObjectConnection *pConnection)
{
	if(!m_pConnectionList)
	{
		m_pConnectionList = new KviKvsObjectConnectionList;
		m_pConnectionList->setAutoDelete(false);
	}
	m_pConnectionList->append(pConnection);
}

bool KviKvsObject::disconnectSignal(const QString &sigName,KviKvsObject * pTarget,const QString &slotName)
{
	if(!m_pSignalDict)return false; //no such signal to disconnect

	KviKvsObjectConnectionList * l = m_pSignalDict->find(sigName);
	if(!l)return false;

	KviKvsObjectConnectionListIterator it(*l);

	while(KviKvsObjectConnection * sl = it.current())
	{
		if(sl->pTargetObject == pTarget)
		{
			if(KviQString::equalCI(sl->szSlot,slotName))
			{
				pTarget->unregisterConnection(sl);
				l->removeRef(sl);
				if(l->isEmpty())m_pSignalDict->remove(sigName);
				if(m_pSignalDict->isEmpty())
				{
					delete m_pSignalDict;
					m_pSignalDict = 0;
				}
				return true;
			}
		}
		++it;
	}
	return false;
}

bool KviKvsObject::disconnectSignal(const QString &sigName,KviKvsObjectConnection * pConnection)
{
	if(!m_pSignalDict)return false;
	KviKvsObjectConnectionList * l = m_pSignalDict->find(sigName);
	//__range_valid(l);
	if(!l)return false;
	pConnection->pTargetObject->unregisterConnection(pConnection);
	//__range_valid(l->findRef(pConnection) > -1);
	l->removeRef(pConnection);
	if(l->isEmpty())m_pSignalDict->remove(sigName);
	if(m_pSignalDict->isEmpty())
	{
		delete m_pSignalDict;
		m_pSignalDict = 0;
	}
	return true;
}

bool KviKvsObject::unregisterConnection(KviKvsObjectConnection * pConnection)
{
	if(!m_pConnectionList)return false;
	bool bOk = m_pConnectionList->removeRef(pConnection); // no auto delete !
	if(!bOk)return false;
	if(m_pConnectionList->isEmpty())
	{
		delete m_pConnectionList;
		m_pConnectionList = 0;
	}
	return true;
}


//FIXME: emitSignal crash whith this script test:
/*
class(A)
{
	emitted()
	{
		debug emitted
		delete -i %C
	}
}
class(C)
{
	emitted()
	{
		debug class C
		delete -I %A
	}
}
class(B)
{
	start()
	{
		@$emit(test)
	}
}
%A=$new(A)
%B=$new(B)
%C=$new(C)

objects.connect %B test %A emitted
objects.connect %B test %C emitted


%B->$start()
*/
int KviKvsObject::emitSignal(const QString &sigName,KviKvsObjectFunctionCall * pOuterCall,KviKvsVariantList * pParams)
{
	if(!m_pSignalDict)return 0;

	KviKvsObjectConnectionList * l = m_pSignalDict->find(sigName);
	if(!l)return 0; // no slots registered

	KviKvsVariant retVal;

	// The objects we're going to disconnect
	KviPointerList<KviKvsObjectConnection> * pDis = 0;

	kvs_int_t emitted = 0;

	KviPointerList<KviKvsObjectConnection> * pTest = new KviPointerList<KviKvsObjectConnection>;
	pTest->setAutoDelete(true);
	for (int i=0;i<l->count();i++) pTest->append(l->at(i));

	KviKvsObjectConnectionListIterator it(*pTest);
//	int connectionsCount=l->count();
//	for (int i=0;i<connectionsCount;i++)
	while(KviKvsObjectConnection * s =it.current() )
	{
	//	KviKvsObjectConnection * s = l->at(i);
		// save it , since s may be destroyed in the call!
	
		debug ("signal %s",sigName.utf8().data());
		KviKvsObject * pTarget = s->pTargetObject;

		kvs_hobject_t hTarget = pTarget->handle();
		kvs_hobject_t hOld = pTarget->signalSender();

		pTarget->setSignalSender(m_hObject);
		pTarget->setSignalName(sigName);

		if(!KviKvsKernel::instance()->objectController()->lookupObject(hTarget))
		{
			//FIXME
			/*pOuterCall->warning("Object no longer exists"
					);
*/
				debug("Object no longer exists");
				/*
				if(!pDis)
				{
					pDis = new KviPointerList<KviKvsObjectConnection>;
					pDis->setAutoDelete(false);
				}
				pDis->append(s);
				*/

		}
		else
		{
			emitted++;

			if(!pTarget->callFunction(this,s->szSlot,QString::null,pOuterCall->context(),&retVal,pParams))
			{
				if(KviKvsKernel::instance()->objectController()->lookupObject(hTarget) && it.current())
				{
					pOuterCall->warning(
						__tr2qs("Broken slot '%Q' in target object '%Q::%Q' while emitting signal '%Q' from object '%Q::%Q': disconnecting"),
						&(s->szSlot),
						&(s->pTargetObject->getClass()->name()),
						&(s->pTargetObject->getName()),
						&(sigName),
						&(getClass()->name()),
						&m_szName);
					if(!pDis)
					{
						pDis = new KviPointerList<KviKvsObjectConnection>;
						pDis->setAutoDelete(false);
					}
					pDis->append(s);

				} 
				else 
				{
			 		// else destroyed in the call! (already disconnected)
					pOuterCall->warning(
						__tr2qs("Slot target object destroyed while emitting signal '%Q' from object '%Q::%Q'"),
						&(sigName),
						&(getClass()->name()),
					&m_szName);
				}
			}
			else pTarget->setSignalSender(hOld);
		}
		/*
		if(KviKvsKernel::instance()->objectController()->lookupObject(hTarget))
		{
			pTarget->setSignalSender(hOld);
	}*/
		++it;
	}

	if(pDis)
	{
		// we have some signals to disconnect (because they're broken)
		for(KviKvsObjectConnection * con = pDis->first();con;con = pDis->next())
			disconnectSignal(sigName,con);
		delete pDis;
	}

	return emitted;
}


bool KviKvsObject::function_name(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setString(getName());
	return true;
}

bool KviKvsObject::function_parent(KviKvsObjectFunctionCall * c)
{
	KviKvsObject * o = parentObject();
	c->returnValue()->setHObject(o ? o->handle() : (kvs_hobject_t)0);
	return true;
}

bool KviKvsObject::function_className(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setString(getClass()->name());
	return true;
}

bool KviKvsObject::function_childCount(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setInteger((kvs_int_t)(m_pChildList->count()));
	return true;
}

bool KviKvsObject::function_signalSender(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setHObject(m_hSignalSender);
	return true;
}

bool KviKvsObject::function_signalName(KviKvsObjectFunctionCall * c)
{
	c->returnValue()->setString(m_szSignalName);
	return true;
}

bool KviKvsObject::function_destructor(KviKvsObjectFunctionCall * c)
{
	emitSignal("destroyed",c);
	return true;
}

bool KviKvsObject::function_children(KviKvsObjectFunctionCall * c)
{
	KviKvsArray * a = new KviKvsArray();
	int id=0;
	for(KviKvsObject * o = m_pChildList->first();o;o = m_pChildList->next())
	{
		a->set(id,new KviKvsVariant(o->handle()));
		id++;
	}
	c->returnValue()->setArray(a);
	return true;
}
bool KviKvsObject::function_findChild(KviKvsObjectFunctionCall * c)
{
	QString szClass,szName;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("className",KVS_PT_STRING,KVS_PF_OPTIONAL,szClass)
		KVSO_PARAMETER("objectName",KVS_PT_STRING,KVS_PF_OPTIONAL,szName)
	KVSO_PARAMETERS_END(c)

	KviKvsObject * o = findChild(szClass,szName);
	c->returnValue()->setHObject(o ? o->handle() : (kvs_hobject_t)0);

	return true;
}

bool KviKvsObject::function_emit(KviKvsObjectFunctionCall * c)
{
	QString szSignal;
	KviKvsVariantList vList;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("signal",KVS_PT_NONEMPTYSTRING,0,szSignal)
		KVSO_PARAMETER("params",KVS_PT_VARIANTLIST,KVS_PF_OPTIONAL,vList)
	KVSO_PARAMETERS_END(c)
	

	emitSignal(szSignal,c,&vList);
	return true;
}

bool KviKvsObject::function_startTimer(KviKvsObjectFunctionCall * c)
{
	kvs_int_t timeout;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("timeout",KVS_PT_UINT,0,timeout)
	KVSO_PARAMETERS_END(c)

	c->returnValue()->setInteger((kvs_int_t)(startTimer(timeout)));
	return true;
}

bool KviKvsObject::function_killTimer(KviKvsObjectFunctionCall * c)
{
	kvs_int_t id;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("timerId",KVS_PT_INT,0,id)
	KVSO_PARAMETERS_END(c)
	killTimer(id);
	return true;
}

bool KviKvsObject::function_killTimers(KviKvsObjectFunctionCall * c)
{
	// FIXME: QT4 does not seem to have QObject::killTimers()
#ifndef COMPILE_USE_QT4
	killTimers();
#endif
	return true;
}

bool KviKvsObject::function_listProperties(KviKvsObjectFunctionCall * c)
{
	bool bArray;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bArray",KVS_PT_BOOL,KVS_PF_OPTIONAL,bArray)
	KVSO_PARAMETERS_END(c)

	c->returnValue()->setNothing();

	KviKvsArray * a = bArray ? new KviKvsArray() : 0;

	KviWindow * w = c->context()->window();

	if(!bArray)
		w->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Listing Qt properties for object named \"%Q\" of KVS class %Q"),&m_szName,&(m_pClass->name()));
	kvs_int_t cnt = 0;
	if(m_pObject)
	{
		const QMetaObject *o = m_pObject->metaObject();
		if(!bArray)
			w->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Properties for Qt class %s"),o->className());
#ifndef COMPILE_USE_QT4
		while(o)
		{
#endif
			kvs_int_t idx = 0;
			#ifdef COMPILE_USE_QT4
				QMetaProperty prop = o->property(idx);
				const QMetaProperty *p = &prop;
			#else
				const QMetaProperty *p = o->property(idx);
			#endif

			while(p)
			{
				QString szOut;
				QString szName = p->name();
				#ifdef COMPILE_USE_QT4
					QString szType = p->typeName();
				#else
				QString szType = p->type();
				#endif
				if(bArray)
					KviQString::sprintf(szOut,"%Q, %Q",&szName,&szType);
				else {
					KviQString::sprintf(szOut,__tr2qs("Property: %c%Q%c, type %Q"),KVI_TEXT_BOLD,&szName,KVI_TEXT_BOLD,&szType);
					szOut.prepend(" ");
				}
				
				if(p->isEnumType())
				{
					szOut += ", enum(";
#ifndef COMPILE_USE_QT4
					// FIXME: Qt 4.x needs QMetaEnum for this loop
					QStrList le = p->enumKeys();
					int i = 0;
					for(char *c2 = le.first(); c2; c2 = le.next())
					{
						if(i == 0)
							i++;
						else
							szOut.append(", ");
						szOut.append(c2);
					}
#endif
					szOut += ")";
				}
				

#ifdef COMPILE_USE_QT4
				// FIXME: QT4 Need to read better the docs and check the changes: there seem to be too many
				//        for me to fix now. Actually I need to get the whole executable working...
				if(p->isWritable())szOut += ", writable";
#else
				if(p->isSetType())szOut += ", set";
				if(p->writable())szOut += ", writable";
#endif
				if(bArray)
					a->set(cnt,new KviKvsVariant(szOut));
				else
					w->outputNoFmt(KVI_OUT_SYSTEMMESSAGE,szOut);
#ifdef COMPILE_USE_QT4
				idx++;
				if (idx<o->propertyCount()){
						prop = o->property(idx);
						p = &prop;
				}
				else p=0;
#else
				p = o->property(idx);
				idx++;
#endif
				
				cnt++;
			}
#ifndef COMPILE_USE_QT4	
			o = o->superClass();
		}
#endif

	}

	if(bArray)
		c->returnValue()->setArray(a);
	else
		w->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("%d properties listed"),cnt);
	return true;
}


// rewritten using the new KVS features :)
bool KviKvsObject::function_setProperty(KviKvsObjectFunctionCall * c)
{
	QString szName;
	KviKvsVariant * v;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("propertyName",KVS_PT_NONEMPTYSTRING,0,szName)
		KVSO_PARAMETER("propertyValue",KVS_PT_VARIANT,0,v)
	KVSO_PARAMETERS_END(c)

	c->returnValue()->setNothing();

	if(!m_pObject)
	{
		// there are no Qt properties at all
		c->warning(__tr2qs("The object named \"%Q\" of class %Q has no Qt properties"),&m_szName,&(m_pClass->name()));
		return true;
	}

#ifdef COMPILE_USE_QT4
	int idx = m_pObject->metaObject()->indexOfProperty(szName);
#else
	int idx = m_pObject->metaObject()->findProperty(szName,true);
#endif
	if(idx < 0)
	{
		c->warning(__tr2qs("No Qt property named \"%Q\" for object named \"%Q\" of class %Q"),&szName,&m_szName,&(m_pClass->name()));
		return true;
	}
#ifdef COMPILE_USE_QT4
	QMetaProperty prop = m_pObject->metaObject()->property(idx);
	const QMetaProperty * p = &prop;
#else
	const QMetaProperty * p = m_pObject->metaObject()->property(idx,true);
#endif
	if(!p)
	{
		c->warning(__tr2qs("Can't find property named \"%Q\" for object named \"%Q\" of class %Q: the property is indexed but it doesn't really exist"),&szName,&m_szName,&(m_pClass->name()));
		return true;
	}

	QVariant vv = m_pObject->property(szName);
	if(!vv.isValid())
	{
		c->warning(__tr2qs("Can't find property named \"%Q\" for object named \"%Q\" of class %Q: the property is indexed and defined but the returned variant is not valid"),&szName,&m_szName,&(m_pClass->name()));
		return true;
	}

	if(p->isEnumType())
	{
		QString szKey;
		v->asString(szKey);
#ifdef COMPILE_USE_QT4
		int val = p->enumerator().keyToValue(szKey);
#else
		int val = p->keyToValue(szKey);
#endif
		QVariant var(val);
		m_pObject->setProperty(szName,var);
		return true;
	}

#define WRONG_TYPE(__therighttype) \
	{ \
 		c->warning(__tr2qs("The property is of type %s but the supplied argument can't be converted to that type (expecting \"%s\")"),p->type(),__therighttype); \
		return true; \
	}

	switch(vv.type())
	{
		case QVariant::Int:
		{
			kvs_int_t i;
			if(!v->asInteger(i))WRONG_TYPE("integer")
			m_pObject->setProperty(szName,QVariant((int)i));
		}
		break;
		case QVariant::UInt:
		{
			kvs_int_t i;
			if(!v->asInteger(i))WRONG_TYPE("unsigned integer")
			if(i < 0)WRONG_TYPE("unsigned integer")
			m_pObject->setProperty(szName,QVariant((unsigned int)i));
		}
		break;
		case QVariant::Bool:
			m_pObject->setProperty(szName,QVariant(v->asBoolean()));
		break;
		case QVariant::String:
		{
			QString s;
			v->asString(s);
			m_pObject->setProperty(szName,QVariant(s));
		}
		break;
		case QVariant::CString:
		{
			QString s;
			v->asString(s);
			m_pObject->setProperty(szName,QVariant(s.utf8()));
		}
		break;
		case QVariant::Point:
		{
			if(!v->isArray())WRONG_TYPE("array(integer,integer)")
			KviKvsArray * a = v->array();
			KviKvsVariant * x = a->at(0);
			KviKvsVariant * y = a->at(1);
			if(!x || !y)WRONG_TYPE("array(integer,integer)")
			kvs_int_t iX,iY;
			if(!x->asInteger(iX) || !y->asInteger(iY))WRONG_TYPE("array(integer,integer)")
			m_pObject->setProperty(szName,QVariant(QPoint(iX,iY)));
		}
		break;
		case QVariant::Size:
		{
			if(!v->isArray())WRONG_TYPE("array(integer,integer)")
			KviKvsArray * a = v->array();
			KviKvsVariant * w = a->at(0);
			KviKvsVariant * h = a->at(1);
			if(!w || !h)WRONG_TYPE("array(integer,integer)")
			kvs_int_t iW,iH;
			if(!w->asInteger(iW) || !h->asInteger(iH))WRONG_TYPE("array(integer,integer)")
			m_pObject->setProperty(szName,QVariant(QSize(iW,iH)));
		}
		break;
		case QVariant::Rect:
		{
			if(!v->isArray())WRONG_TYPE("array(integer,integer,integer,integer)")
			KviKvsArray * a = v->array();
			KviKvsVariant * x = a->at(0);
			KviKvsVariant * y = a->at(1);
			KviKvsVariant * w = a->at(2);
			KviKvsVariant * h = a->at(3);
			if(!x || !y || !w || !h)WRONG_TYPE("array(integer,integer,integer,integer)")
			kvs_int_t iX,iY,iW,iH;
			if(!x->asInteger(iX) || !y->asInteger(iY) || !w->asInteger(iW) || !h->asInteger(iH))WRONG_TYPE("array(integer,integer,integer,integer)")
			m_pObject->setProperty(szName,QVariant(QRect(iX,iY,iW,iH)));
		}
		break;
#ifndef COMPILE_USE_QT4
		// FIXME: QT4 ????
		case QVariant::Color:
		{
			if(!v->isArray())WRONG_TYPE("array(integer,integer,integer)")
			KviKvsArray * a = v->array();
			KviKvsVariant * r = a->at(0);
			KviKvsVariant * g = a->at(1);
			KviKvsVariant * b = a->at(3);
			if(!r || !g || !b)WRONG_TYPE("array(integer,integer,integer)")
			kvs_int_t iR,iG,iB;
			if(!r->asInteger(iR) || !g->asInteger(iG) || !b->asInteger(iB))WRONG_TYPE("array(integer,integer,integer)")
			m_pObject->setProperty(szName,QVariant(QColor(iR,iG,iB)));
		}
		break;
		case QVariant::Font:
		{
			if(!v->isArray())WRONG_TYPE("array(string,integer,string)")
			KviKvsArray * a = v->array();
			KviKvsVariant * ff = a->at(0);
			KviKvsVariant * ps = a->at(1);
			KviKvsVariant * fl = a->at(3);
			if(!ff || !ps)WRONG_TYPE("array(string,integer,string)")
			kvs_int_t iPs;
			if(!ps->asInteger(iPs))WRONG_TYPE("array(string,integer,string)")
			QString szFf,szFl;
			ff->asString(szFf);
			if(fl)fl->asString(szFl);
			QFont fnt;
			fnt.setFamily(szFf);
			fnt.setPointSize(iPs);
			if(szFl.find('b') != -1)fnt.setBold(true);
			if(szFl.find('i') != -1)fnt.setItalic(true);
			if(szFl.find('u') != -1)fnt.setUnderline(true);
			if(szFl.find('o') != -1)fnt.setOverline(true);
			if(szFl.find('f') != -1)fnt.setFixedPitch(true);
			if(szFl.find('s') != -1)fnt.setStrikeOut(true);
			m_pObject->setProperty(szName,QVariant(fnt));
		}
		break;
		case QVariant::Pixmap:
		case QVariant::IconSet:
		{
			if(v->isHObject())
			{
				if(v->hobject() == (kvs_hobject_t)0)
				{
					// null pixmap
					if(vv.type() == QVariant::Pixmap)
						m_pObject->setProperty(szName,QVariant(QPixmap()));
					else
						m_pObject->setProperty(szName,QVariant(QIconSet()));
				} else {
					KviKvsObject * pix = KviKvsKernel::instance()->objectController()->lookupObject(v->hobject());
					if(!pix->inherits("KviScriptPixmapObject"))
						c->warning(__tr2qs("A pixmap object, an image_id or an image file path is required for this property"));
					else {
						QVariant pixv = pix->property("pixmap");
						if(vv.type() == QVariant::Pixmap)
							m_pObject->setProperty(szName,pixv);
						else
							m_pObject->setProperty(szName,QVariant(QIconSet(pixv.toPixmap())));
					}
				}
			} else {
				QString szStr;
				v->asString(szStr);
				QPixmap * pPix = g_pIconManager->getImage(szStr);
				if(pPix)
				{
					if(vv.type() == QVariant::Pixmap)
						m_pObject->setProperty(szName,QVariant(*pPix));
					else
						m_pObject->setProperty(szName,QVariant(QIconSet(*pPix)));
				}
				else
					c->warning(__tr2qs("Can't find the requested image"));
			}
		}
		break;
#endif
		default:
			c->warning(__tr2qs("Property \"%Q\" for object named \"%Q\" of class %Q has an unsupported data type"),&szName,&m_szName,&(m_pClass->name()));
			c->returnValue()->setNothing();
		break;
	}
	return true;
}

bool KviKvsObject::function_property(KviKvsObjectFunctionCall * c)
{
	QString szName;
	bool bNoerror;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("propertyName",KVS_PT_NONEMPTYSTRING,0,szName)
		KVSO_PARAMETER("bNowarning",KVS_PT_BOOL,KVS_PF_OPTIONAL,bNoerror)
	KVSO_PARAMETERS_END(c)

	if(!m_pObject)
	{
		// there are no Qt properties at all
		if (bNoerror) c->returnValue()->setString("No Qt properties");
		else
		{
			c->warning(__tr2qs("The object named \"%Q\" of class %Q has no Qt properties"),&m_szName,&(m_pClass->name()));
			c->returnValue()->setNothing();
		}
		return true;
	}

#ifdef COMPILE_USE_QT4
	int idx = m_pObject->metaObject()->indexOfProperty(szName);
#else
	int idx = m_pObject->metaObject()->findProperty(szName,true);
#endif
	if(idx < 0)
	{
		if (bNoerror) c->returnValue()->setString("No Qt properties");
		else
		{
			c->warning(__tr2qs("No Qt property named \"%Q\" for object named \"%Q\" of class %Q"),&szName,&m_szName,&(m_pClass->name()));
			c->returnValue()->setNothing();
		}
		return true;
	}
#ifdef COMPILE_USE_QT4
	QMetaProperty prop = m_pObject->metaObject()->property(idx);
	const QMetaProperty * p = &prop;
#else
	const QMetaProperty * p = m_pObject->metaObject()->property(idx,true);
#endif
	if(!p)
	{
		c->warning(__tr2qs("Can't find property named \"%Q\" for object named \"%Q\" of class %Q: the property is indexed but it doesn't really exist"),&szName,&m_szName,&(m_pClass->name()));
		c->returnValue()->setNothing();
		return true;
	}

	QVariant v = m_pObject->property(szName);
	if(!v.isValid())
	{
		c->warning(__tr2qs("Can't find property named \"%Q\" for object named \"%Q\" of class %Q: the property is indexed and defined but the returned variant is not valid"),&szName,&m_szName,&(m_pClass->name()));
		c->returnValue()->setNothing();
		return true;
	}

	if(p->isEnumType())
	{
#ifdef COMPILE_USE_QT4
		c->returnValue()->setString(p->enumerator().valueToKey(v.toInt()));
#else
		c->returnValue()->setString(p->valueToKey(v.toInt()));
#endif
		return true;
	}

	switch(v.type())
	{
		case QVariant::Int:
			c->returnValue()->setInteger((kvs_int_t)v.toInt());
		break;
		case QVariant::UInt:
			c->returnValue()->setInteger((kvs_int_t)v.toUInt());
		break;
		case QVariant::Bool:
			c->returnValue()->setBoolean(v.toBool());
		break;
		case QVariant::String:
			c->returnValue()->setString(v.toString());
		break;
		case QVariant::CString:
			c->returnValue()->setString(QString::fromUtf8(v.toCString().data()));
		break;
		case QVariant::Point:
		{
			QPoint p = v.toPoint();
			KviKvsArray * a = new KviKvsArray();
			a->set(0,new KviKvsVariant((kvs_int_t)p.x()));
			a->set(1,new KviKvsVariant((kvs_int_t)p.y()));
			c->returnValue()->setArray(a);
		}
		break;
		case QVariant::Size:
		{
			QSize p = v.toSize();
			KviKvsArray * a = new KviKvsArray();
			a->set(0,new KviKvsVariant((kvs_int_t)p.width()));
			a->set(1,new KviKvsVariant((kvs_int_t)p.height()));
			c->returnValue()->setArray(a);
		}
		break;
		case QVariant::Rect:
		{
			QRect p = v.toRect();
			KviKvsArray * a = new KviKvsArray();
			a->set(0,new KviKvsVariant((kvs_int_t)p.x()));
			a->set(1,new KviKvsVariant((kvs_int_t)p.y()));
			a->set(2,new KviKvsVariant((kvs_int_t)p.width()));
			a->set(3,new KviKvsVariant((kvs_int_t)p.height()));
			c->returnValue()->setArray(a);
		}
		break;
#ifndef COMPILE_USE_QT4
		// FIXME: QT4 ?
		case QVariant::Color:
		{
			QColor clr = v.toColor();
			KviKvsArray * a = new KviKvsArray();
			a->set(0,new KviKvsVariant((kvs_int_t)clr.red()));
			a->set(1,new KviKvsVariant((kvs_int_t)clr.green()));
			a->set(2,new KviKvsVariant((kvs_int_t)clr.blue()));
			c->returnValue()->setArray(a);
		}
		break;
		case QVariant::Font:
		{
			QFont f = v.toFont();
			KviKvsArray * a = new KviKvsArray();
			a->set(0,new KviKvsVariant(f.family()));
			a->set(1,new KviKvsVariant((kvs_int_t)f.pointSize()));
			QString szFlags;
			if(f.bold())szFlags += "b";
			if(f.underline())szFlags += "u";
			if(f.overline())szFlags += "o";
			if(f.strikeOut())szFlags += "s";
			if(f.fixedPitch())szFlags += "f";
			if(f.italic())szFlags += "i";
			a->set(2,new KviKvsVariant(szFlags));
			c->returnValue()->setString(szFlags);
		}
		break;
#endif
		default:
			if (bNoerror) c->returnValue()->setString("Unsupported_data_type");
			else
			{
				c->warning(__tr2qs("Property \"%Q\" for object named \"%Q\" of class %Q has an unsupported data type"),&szName,&m_szName,&(m_pClass->name()));
				c->returnValue()->setNothing();
			}
		break;
	}
	return true;
}

void KviKvsObject::killAllChildrenWithClass(KviKvsObjectClass *cl)
{
	KviPointerList<KviKvsObject> l;
	l.setAutoDelete(true);
	for(KviKvsObject * o=m_pChildList->first();o;o=m_pChildList->next())
	{
		if(o->getClass() == cl)
		{
			l.append(o);
		} else o->killAllChildrenWithClass(cl);
	}
}

bool KviKvsObject::inheritsClass(KviKvsObjectClass * pClass)
{
	if(pClass == m_pClass)return true;
	KviKvsObjectClass * cl = m_pClass->m_pParentClass;
	while(cl)
	{
		if(cl == pClass)return true;
		else cl = cl->m_pParentClass;
	}
	return false;
}

KviKvsObjectClass * KviKvsObject::getClass(const QString & classOverride)
{
	if(classOverride.isEmpty())return m_pClass;
	KviKvsObjectClass * cl = m_pClass; // class override can be also THIS class
	// if object->$function() is a local override, class::object->$function()
	// is the class member function (not the local override)
	while(cl)
	{
		if(KviQString::equalCI(cl->name(),classOverride))break;
		else cl = cl->m_pParentClass;
	}
	return cl;
}

KviKvsObjectFunctionHandler * KviKvsObject::lookupFunctionHandler(const QString & funcName,const QString & classOverride)
{
	KviKvsObjectFunctionHandler * h = 0;

	if(classOverride.isEmpty() && m_pFunctionHandlers)
	{
		// lookup the local overrides
		h = m_pFunctionHandlers->find(funcName);
	}

	if(!h)
	{
		// not a local override function... lookup in the class
		KviKvsObjectClass * cl = getClass(classOverride);
		if(cl)return cl->lookupFunctionHandler(funcName);
	}

	return h;
}


bool KviKvsObject::die()
{
	if(m_bInDelayedDeath)return false;
	m_bInDelayedDeath = true;
	QTimer::singleShot(0,this,SLOT(delayedDie()));
	return true;
}

bool KviKvsObject::dieNow()
{
	if(m_bInDelayedDeath)return false;
	m_bInDelayedDeath = true;
	delete this;
	return true;
}

void KviKvsObject::delayedDie()
{
	delete this; // byez!
}

void KviKvsObject::setObject(QObject * o,bool bIsOwned)
{
	//__range_invalid(m_pObject);
	m_bObjectOwner = bIsOwned;
	m_pObject = o;
	o->installEventFilter(this);
	connect(m_pObject,SIGNAL(destroyed()),this,SLOT(objectDestroyed()));
}

void KviKvsObject::objectDestroyed()
{
	m_pObject = 0;
	die();
}

bool KviKvsObject::eventFilter(QObject *o,QEvent *e)
{
	return false; // do not stop
}

void KviKvsObject::timerEvent(QTimerEvent *e)
{
	KviKvsVariant * v = new KviKvsVariant();
	v->setInteger(e->timerId());
	KviKvsVariantList parms(v);

	callFunction(this,"timerEvent",&parms);
}

bool KviKvsObject::callFunction(KviKvsObject * pCaller,const QString &fncName,KviKvsVariant * pRetVal,KviKvsVariantList * pParams)
{
	KviKvsVariant rv;
	if(!pRetVal)pRetVal = &rv;
	KviKvsRunTimeContext ctx(0,g_pApp->activeConsole(),KviKvsKernel::instance()->emptyParameterList(),pRetVal,0);
	if(!pParams)pParams = KviKvsKernel::instance()->emptyParameterList();
	return callFunction(pCaller,fncName,QString::null,&ctx,pRetVal,pParams);
}


bool KviKvsObject::callFunction(KviKvsObject * pCaller,const QString &fncName,KviKvsVariantList * pParams)
{
	KviKvsVariant fakeRetVal;
	return callFunction(pCaller,fncName,&fakeRetVal,pParams);
}

bool KviKvsObject::callFunction(
	KviKvsObject * pCaller,
	const QString & fncName,
	const QString & classOverride,
	KviKvsRunTimeContext * pContext,
	KviKvsVariant * pRetVal,
	KviKvsVariantList * pParams)
{
	KviKvsObjectFunctionHandler * h = lookupFunctionHandler(fncName,classOverride);

	if(!h)
	{
		if(classOverride.isEmpty())
			pContext->error(__tr2qs("Cannot find object function $%Q for object named \"%Q\" of class %Q"),&fncName,&m_szName,&(getClass()->name()));
		else
			pContext->error(__tr2qs("Cannot find object function $%Q::%Q for object named \"%Q\" of class %Q"),&classOverride,&fncName,&m_szName,&(getClass()->name()));
		return false;
	}

	if(h->flags() & KviKvsObjectFunctionHandler::Internal)
	{
		if(pCaller != this)
		{
			pContext->error(__tr2qs("Cannot call internal object function $%Q (for object named \"%Q\" of class %Q) from this context"),&fncName,&m_szName,&(getClass()->name()));
			return false;
		}
	}

	KviKvsObjectFunctionCall fc(pContext,pParams,pRetVal);

	return h->call(this,&fc);

	// Not only gcc spits out compiler errors:
	// 25.09.2001 , at this point in file

	// c:\programmi\microsoft visual studio\myprojects\kvirc3\src\kvirc\uparser\kvi_scriptobject.cpp(1234) : fatal error C1001: INTERNAL COMPILER ERROR
	//    (compiler file 'E:\8168\vc98\p2\src\P2\main.c', line 494)
	//    Please choose the Technical Support command on the Visual C++
	//    Help menu, or open the Technical Support help file for more information
}



void KviKvsObject::registerPrivateImplementation(const QString &szFunctionName,const QString &szCode)
{
	if(szCode.isEmpty())
	{
		if(m_pFunctionHandlers)
		{
			m_pFunctionHandlers->remove(szFunctionName);
			if(m_pFunctionHandlers->isEmpty())
			{
				delete m_pFunctionHandlers;
				m_pFunctionHandlers = 0;
			}
		}
	} else {
		if(!m_pFunctionHandlers)
		{
			m_pFunctionHandlers = new KviPointerHashTable<QString,KviKvsObjectFunctionHandler>(7,false);
			m_pFunctionHandlers->setAutoDelete(true);
		}

		QString szContext = m_pClass->name();
		szContext += "[privateimpl]::";
		szContext += szFunctionName;

		m_pFunctionHandlers->replace(szFunctionName,new KviKvsObjectScriptFunctionHandler(szContext,szCode));
	}
}


KviKvsObject * KviKvsObject::findChild(const QString &szClass,const QString &szName)
{
	for(KviKvsObject * o = m_pChildList->first();o;o= m_pChildList->next())
	{
		if(szClass.isEmpty())
		{
			// any class matches
			if(szName.isEmpty())return o; // any name matches
			// name must match
			if(KviQString::equalCI(szName,o->name()))return o;
		} else {
			if(KviQString::equalCI(szClass,o->getClass()->name()))
			{
				if(szName.isEmpty())return o; // any name matches
				// name must match
				if(KviQString::equalCI(szName,o->name()))return o;
			}
		}
		KviKvsObject * c = o->findChild(szClass,szName);
		if(c)return c;
	}
	return 0;
}


