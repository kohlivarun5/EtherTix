[@bs.module] external qr_reader: ReasonReact.reactClass = "react-qr-reader";

[@bs.deriving abstract]
type jsProps = {
  delay:int,
  facingMode:string,
  onScan:(Js.Option.t(string) => unit),
  onError:(string => unit),
  style:ReactDOMRe.style
};

let make = (~onScan,~onError,~style,children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=qr_reader,
    ~props=jsProps(~style,~delay=500,~facingMode="environment",~onScan,~onError),
    children,
  );

