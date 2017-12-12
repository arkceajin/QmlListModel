import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.1

Window {
    visible: true
    width: 360
    height: 360

    Column {
        x: 10; y: 10
        spacing: 10
        Text {
            font.weight: Font.Bold
            font.pixelSize: 24
            text: "Company :"
        }
        Repeater{
            model: companyModel
            delegate: Column{
                spacing: 10
                Row{
                    spacing: 10
                    Text {
                        font.pixelSize: 20
                        text: apartmentName
                    }
                    TextField{
                        id: inputName
                        placeholderText: "Input name"
                    }
                    Button{
                        text: "Add Member"
                        onClicked: {
                            if(inputName.text != ""){
                                var newName = inputName.text,
                                    existed = false
                                // Iteration
                                for(var i = 0; i < members.size(); i++){
                                    // Access data
                                    if(members.get(i).memberName == newName)
                                        existed = true
                                }
                                if(!existed)
                                    members.addMember(newName)
                            }
                        }
                    }
                }
                Repeater{
                    model: members
                    delegate: Text {
                        x: 10
                        font.pixelSize: 20
                        text: memberName
                    }
                }
            }
        }

    }
}
