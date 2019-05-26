
import 'package:flutter/material.dart';
import 'package:dio/dio.dart';

void main(){
  runApp(new MaterialApp(
    home: new MyButton(),
  ));
}

class MyButton extends StatefulWidget{
  @override
  MyButtonState createState(){
    return new MyButtonState();
  }
}

class MyButtonState extends State<MyButton>{
  int count = 0;
  List<String> strings = ['flutter','is','cool','and','awesome'];
  String displayString = "Hello World";
  void onPressOfButton(){
    setState(() {
      displayString = strings[count];
      count = count<4 ? count+1:0;
    });
  }

  @override
  Widget build(BuildContext context){
    return new Scaffold(
      appBar: new AppBar(
        title: new Text("Stateful widget"),
        backgroundColor: Colors.green,
      ),
      body: new Container(
        child: new Center(
          child: new Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              new Text(displayString, style: new TextStyle(fontSize: 40.0)),
              new Padding(padding: new EdgeInsets.all(10.0),),
              new RaisedButton(
                child: new Text("Turn On", style: new TextStyle(color:Colors.white)),
                color: Colors.red,
                onPressed: ()=>_turnOnOff("on"),
              ),
              new RaisedButton(
                child: new Text("Turn Off", style: new TextStyle(color:Colors.white)),
                color: Colors.red,
                onPressed: ()=>_turnOnOff("off"),
              )
            ],
          ),
        ),
      ),
    );
  }

}

_turnOnOff(command) async {
  Dio dio = new Dio();
  await dio.post("http://127.0.0.1:6666/command",data:{"topic":"airconditionertopic","message":command});
}
