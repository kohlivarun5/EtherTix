let text = ReasonReact.string;

let component = ReasonReact.statelessComponent("QrView");

let make = (~text,~style,_children) => {
  ...component,
  didMount: self => { 
    let canvas = 
      QrCode.document 
      |> QrCode.getElementById(Js.String.concat(text,"QrView:"));
    QrCode.toDataURL(canvas,text);
    ()
  },
  render: ({send,state}) => <canvas style=style id=(Js.String.concat(text,"QrView:")) />
}
