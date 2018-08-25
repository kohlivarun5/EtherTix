let text = ReasonReact.string;

let component = ReasonReact.statelessComponent("QrView");

let make = (~text,_children) => {
  ...component,
  didMount: self => { 
    let canvas = 
      QrCode.document 
      |> QrCode.getElementById(Js.String.concat(text,"QrView:"));
    QrCode.toDataURL(canvas,text);
    ()
  },
  render: ({send,state}) => 
    <div style=(ReactDOMRe.Style.make(~textAlign="center",()))>
      <canvas id=(Js.String.concat(text,"QrView:")) />
    </div>
}
