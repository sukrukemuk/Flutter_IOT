import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:vibration/vibration.dart';

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key});

  @override
  // ignore: library_private_types_in_public_api
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  late DatabaseReference _motionSensorRef;
  late DatabaseReference _waterLevelRef;
  String _waterLevelValue = 'Veri Yükleniyor...';
  Color _waterLevelColor = Colors.green;
  Color _securityIconColor = Colors.green;
  
  @override
  void initState() {
    super.initState();
    _motionSensorRef = FirebaseDatabase.instance.ref().child('test/motion_sensor');
    _waterLevelRef = FirebaseDatabase.instance.ref().child('test/water_level');
    _motionSensorRef.onValue.listen((event) {
      setState(() {
        dynamic value = event.snapshot.value;
        if (value is int) {
          if (value == 1) {
            _securityIconColor = Colors.red;
            _sendVibration();
          } else {
            _securityIconColor = Colors.green;
          }
        } else {
          _securityIconColor = Colors.yellow;
        }
      });
    }, onError: (Object? error) {
      setState(() {
        _securityIconColor = Colors.yellow;
      });
    });
    _waterLevelRef.onValue.listen((event) {
      setState(() {
        dynamic value = event.snapshot.value;
        if (value is int) {
          _waterLevelValue = 'SU SEVİYESİ';
          _waterLevelColor = getColor(value);
        } else {
          _waterLevelValue = 'Veri formatı hatalı';
          _waterLevelColor = Colors.yellow;
        }
      });
    }, onError: (Object? error) {
      setState(() {
        _waterLevelValue = 'Hata: $error';
        _waterLevelColor = Colors.yellow;
      });
    });
  }

  Future<void> _sendVibration() async {
    bool? hasVibrator = await Vibration.hasVibrator();
    if (hasVibrator == true) {
      Vibration.vibrate(duration: 1000);
    }
  }

  Color getColor(int value) {
    if (value >= 0 && value < 500) {
      return Colors.green;
    } else if (value >= 500 && value < 1500) {
      return Colors.blue;
    } else if (value >= 1500 && value < 2000) {
      return Colors.yellow;
    } else if (value >= 2000 && value < 2500) {
      return Colors.orange;
    } else {
      return Colors.red;
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.grey[800],
      body: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: <Widget>[
          Expanded(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                const SizedBox(height: 40),
                const Text(
                  'GÜVENLİK',
                  style: TextStyle(color: Colors.white, fontSize: 45, fontWeight: FontWeight.bold),
                ),
                const SizedBox(height: 25),
                Container(
                  padding: const EdgeInsets.all(20),
                  decoration: BoxDecoration(
                    color: _securityIconColor,
                    borderRadius: BorderRadius.circular(10),
                  ),
                  child: const Icon(
                    Icons.security,
                    size: 180,
                    color: Colors.white,
                  ),
                ),
              ],
            ),
          ),
          Expanded(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                const SizedBox(),
                Text(
                  _waterLevelValue,
                  textAlign: TextAlign.center,
                  style: const TextStyle(color: Colors.white, fontSize: 45, fontWeight: FontWeight.bold),
                ),
                Padding(
                  padding: const EdgeInsets.only(top: 20),
                  child: Icon(
                    Icons.water_drop,
                    size: 270,
                    color: _waterLevelColor,
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
