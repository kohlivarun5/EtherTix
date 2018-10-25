let text = ReasonReact.string;

let component = ReasonReact.statelessComponent("QrView");

let make = (~text,~style,_children) => {
  ...component,
  didMount: (_) => { 
    let canvas = 
      QrCode.document 
      |> QrCode.getElementById(Js.String.concat(text,"QrView:"));
    QrCode.toDataURL(canvas,text);
    ()
  },
  render: (_) => <canvas style=style id=(Js.String.concat(text,"QrView:")) />
}
