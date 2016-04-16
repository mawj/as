---
layout: post
title: AUTOSAR BOOTLOADER
category: real embeded world of automotive
comments: true
---

在汽车电子，现在ｂｏｏｔｌｏａｄｅｒ基本已成汽车ＥＣＵ必备，其负责应用程序的启动和通过ＣＡＮ总线来更新应用程序和数据（如ＮＶＭ）等。

一般而言，ｂｏｏｔｌｏａｄｅｒ通过ＣＡＮ总线来更新应用程序，有些也会通过ＬＩＮ或者ＵＡＲＴ来更新程序，不管通过同种方式，都可以基于[ｉｓｏ１４２２９](https://en.wikipedia.org/wiki/Unified_Diagnostic_Services)来实现，也就是说，ｉｓｏ１４２２９　ＵＤＳ协议算是一种上层应用协议，可用在多种通讯总线之上，而[ｉｓｏ１５７６５](https://en.wikipedia.org/wiki/ISO_15765-2)是基于ＣＡＮ总线的ＵＤＳ协议服务，其定义了如何通过ＣＡＮ总线来传输大数据。

这里只讨论基于ＣＡＮ总线的ｂｏｏｔｌｏａｄｅｒ，说简单点，ｂｏｏｔｌｏａｄｅｒ的主要功能是负责应用程序的启动和程序的更新，而更新过程由一系列的ＵＤＳ服务组合起来实现的，主要由以下８条服务组成。OK,说道这里顺便提一下，目前已经有一个很成熟的ｂｏｏｔｌｏａｄｅｒ规范叫[ＨＩＳ](http://portal.automotive-his.de)，其基于ＡＵＴＯＳＡＲ，对一个ｂｏｏｔｌｏａｄｅｒ该有的元素都做了很详细的定义，有兴趣的可以去了解下，看看他们的文档，不过最近不知道为什么，其官网访问不了，估计又被屏蔽了吧，哎，在党国不会翻墙，ｇｏｏｇｌｅ会永远使用不了，百度还是个渣，郁闷。

##１　Ｓｅｓｓｉｏｎ　Ｃｏｎｔｒｏｌ（会话控制　０ｘ１０）

##1.1 Request message definition

<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>Diagnostic Session Control Request Serivce ID</td>
      <td>10</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>sub-function=[diagnostic session type]</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

##1.2 Request message sub-function parameter $Level (LEV_) definition

<table>
  <thead>
    <tr>
      <th>１６进制值(6位)</th>
      <th>描述</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>０１</td>
      <td>默认会话层(Ｄｅｆａｕｌｔ　Ｓｅｓｓｉｏｎ)</td>
    </tr>
    <tr>
      <td>０２</td>
      <td>编程会话层(Ｐｒｏｇｒａｍｉｎｇ　Ｓｅｓｓｉｏｎ)<br>　这个就是ｂｏｏｔｌｏａｄｅｒ主要要实现的一个会话层，在该会话层下要实现一系列的诊断服务，从而形成一个为升级应用程序提供服务的服务器，ＰＣ或手持客户机即可通过合法访问该会话层下的服务来实现应用程序的更新。<br></td>
    </tr>
    <tr>
      <td>０３</td>
      <td>扩展会话层(Ｅｘｔｅｎｄｅｄ　Ｓｅｓｓｉｏｎ)</td>
    </tr>
  </tbody>
</table>

#1.3 Positive response message

<table>
  <thead>
    <tr>
      <th>索引</th>
      <th>参数名称</th>
      <th>１６进制值</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>#1</td>
      <td>Diagnostic Session Control Response Serivce ID</td>
      <td>50</td>
    </tr>
    <tr>
      <td>#2</td>
      <td>diagnostic session type</td>
      <td>00-7F</td>
    </tr>
    <tr>
      <td>#3 .. #n</td>
      <td>sessionParameterRecord[] #1 = [data1 .. datan]</td>
      <td>00-FF</td>
    </tr>
  </tbody>
</table>

一般而言，当ＥＣＵ上电后，首先执行的是其类似于BIOS的一段程序，该程序一般由芯片厂商设计并不可改写，其会加载后续客户应用程序，对于汽车级ＥＣＵ，其一般为ｂｏｏｔｌｏａｄｅｒ，ｂｏｏｔｌｏａｄｅｒ首先会检查是否有更新程序请求，如果没有，其将检查应用程序的有效性，如果有效，其将启动应用程序。在应用程序运行过程中，如果有客户机通过合法途径（如通过了安全访问）请求进入编程会话层，则应用程序会写一个标记，根据ＨＩＳ规范，该标记会写到ＥＥＰＲＯＭ里面，这样在应用程序执行复位操作之后，ｂｏｏｔｌｏａｄｅｒ会去ＥＥＰＲＯＭ里面读取该标记，即知有更新程序请求，从而停留在ｂｏｏｔｌｏａｄｅｒ，启动编程服务器（Ｓｅｒｖｅｒ），提供一系列的应用程序更新相关的诊断服务给客户机（Ｃｌｉｅｎｔ）来实现程序的升级操作。但是，使用ＥＥＰＲＯＭ来保存升级请求的方式，是开销比较大的，并且也是应用程序的启动时间变长，毕竟要去通过ＳＰＩ／ＩＩＣ等总线去ＥＥＰＲＯＭ读取数据，即使是内部ＥＥＰＲＯＭ数据，也会是比较慢的。所以一本会使用ＭＣＵ内部在不掉点情况下，既使软件复位其值也不会丢失的内存或者寄存器来存储应用程序更新请求标记，相当的简单快捷。另外，在ｂｏｏｔｌｏａｄｅｒ启动过程中，如果检测到应用程序无效，其也会停留在ｂｏｏｔｌｏａｄｅｒ，启动服务器，等待客户机的连接并更新程序。

##２　Ｓｅｃｕｒｉｔｙ　Ａｃｃｅｓｓ（安全访问　０ｘ２７）

好吧，我又犯懒癌了，用ＭＡＲＫＤＯＷＮ的ＨＴＭＬ方式来写表格是一件好麻烦的事情，所以就不在描述消息的格式了。网上搜索ｉｓｏ１４２２９下载其文档看吧。对于ｂｏｏｔｌｏａｄｅｒ安全访问是必须的，从而保证应用程序不被非法客户机给破坏了。安全访问一般分为２步，第一步请求种子（Ｒｅｑｕｅｓｔ　Ｓｅｅｄ）和发送钥匙(Ｓｅｎｄ　Ｋｅｙ)。一般，请求的种子为４字节，是由Ｓｅｒｖｅｒ根据某种特殊算法随机生成，每次访问都会不一样，以防止恶意客户机找到规律给破解了。客户机收到种子之后，经过双方约定的算法来解密，算出钥匙，然后发送给服务器，如果钥匙有效则解锁成功，和此次解锁相关的服务就可以访问了。

目前，一般而言每个会话层就一个安全等级。但ＡＵＴＯＳＡＲ　ＤＣＭ的描述不限于此，同一会话层下，可以有好多个安全等级，举例来说某些服务需要需要使用安全等级１的算法来解锁，又有其他一些服务需要安全等级２来访问，等等，这里要说每个等级没有高低之分，他们都是相等权重的，但等级０意味着不需要解锁，因为这是默认服务器的安全等级。

对于安全等级ｎ，其访问数据格式如下：

* 客户机请求种子：　[２７，２ｎ＋１］

* 服务器返回种子：　[６７，２ｎ＋１，ＸＸ，ＸＸ，ＸＸ，ＸＸ］

* 客户机发送钥匙：　[２７，２ｎ＋２，ＹＹ，ＹＹ，ＹＹ，ＹＹ］

* 服务器解锁成功：　[６７，２ｎ＋２］

##３　Ｒｏｕｔｉｎｅ　Ｃｏｎｔｒｏｌ（过程控制　０ｘ３１）

##４　Ｒｅｑｕｅｓｔ　Ｄｏｗｎｌｏａｄ（请求下载　０ｘ３４）

##５　Ｒｅｑｕｅｓｔ　Ｕｐｌｏａｄ（请求上载　０ｘ３５）

##６　Ｔｒａｎｓｆｅｒ　Ｄａｔａ（传输数据　０ｘ３６）

##７　Ｒｅｑｕｅｓｔ　Ｔｒａｎｓｆｅｒ　Ｅｘｉｔ（请求传输结束　０ｘ３７）

##８　Ｅｃｕ　Ｒｅｓｅｔ（ＥＣＵ复位　０ｘ１１）